#include "DNSApp.h"

#include "inet/applications/tcpapp/GenericAppMsg_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/socket/SocketTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"
#include "inet/common/packet/Message_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"

#include "messages/ObjectMsg_m.h"
#include "messages/LoadDataMsg_m.h"
#include "messages/DNSResponse_m.h"
#include "messages/DNSRequest_m.h"
#include "messages/CacheModificationResponse_m.h"
#include "messages/CacheModificationRequest_m.h"
#include "definitions.h"

#include <cstdio>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace inet;

Define_Module(DNSApp);

void DNSApp::initialize(int stage) {
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        // statistics
        msgsRcvd = msgsSent = bytesRcvd = bytesSent = 0;

        WATCH(msgsRcvd);
        WATCH(msgsSent);
        WATCH(bytesRcvd);
        WATCH(bytesSent);
    } else if (stage == INITSTAGE_APPLICATION_LAYER) {
        socket.setOutputGate(gate("socketOut"));
        socket.bind(L3Address(), 1000);
        socket.listen();

        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus =
                node ? check_and_cast_nullable<NodeStatus*>(
                               node->getSubmodule("status")) :
                       nullptr;
        bool isOperational = (!nodeStatus)
                || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError(
                    "This module doesn't support starting in node DOWN state");

    } else if (stage == INITSTAGE_LAST) {
        cachePolicy = getSystemModule()->par("cachePolicy").intValue();
        numSurrogates = getSystemModule()->par("numSurrogateServers");

        for (int i = 0; i < numSurrogates; i++) {
            serverLoads.push_back(0);
            serverCaches.push_back(unordered_set<int>());
        }

        loadOtherSurrogates();
    }
}

/**
 * load file where for each clientId a surrogateId is given
 */
void DNSApp::loadOtherSurrogates() {
    otherSurrogatesSortedByDistance.resize(numSurrogates);
    ifstream inputFile(par("pathToSurrogateDistances").stringValue());
    if (!inputFile.is_open())
        throw cRuntimeError("Traffic file did not open");

    string line;
    while (getline(inputFile, line)) {
        istringstream iss(line);

        short from, to;
        if (!(iss >> from >> to)) {
            continue;
        }
        otherSurrogatesSortedByDistance[from].push_back(to);
    }
    inputFile.close();
}
/*
* send packet
*/
void DNSApp::sendBack(cMessage *msg) {
    Packet *packet = dynamic_cast<Packet*>(msg);

    if (packet) {
        msgsSent++;
        bytesSent += packet->getByteLength();
        emit(packetSentSignal, packet);
    }

    auto &tags = check_and_cast<ITaggedObject*>(msg)->getTags();
    tags.addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::tcp);
    send(msg, "socketOut");
}

void DNSApp::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        sendBack(msg);

    } else if (msg->getKind() == TCP_I_PEER_CLOSED) {
        handlePeerClosed(msg);
    } else if (msg->getKind() == TCP_I_DATA
            || msg->getKind() == TCP_I_URGENT_DATA) {

        /**
         * This is only the service for surrogate Servers. No service for clients because they already know their closest surrogate server
         */

        ISocket *arrivalSocket = surrogateSocketMap.findSocketFor(msg);

        Packet *packet = check_and_cast<Packet*>(msg);
        const auto &payload = packet->peekAtFront();

        if (arrivalSocket == nullptr)
            throw cRuntimeError("No socket exists for this message");

        if (strcmp(payload->getClassName(), "inet::DNSRequest") == 0) {
            // a surrogate server wants to know where to get object from
            handleSurrogateRequest(arrivalSocket, msg);
        } else if (strcmp(payload->getClassName(), "inet::LoadDataMsg") == 0) {
            // a surrogate server tell the DNS about its current load
            handleLoadMessage(arrivalSocket, msg);
        } else if (strcmp(payload->getClassName(),
                "inet::CacheModificationRequest") == 0) {
            // a surrogate server modified its cache
            handleCacheModification(arrivalSocket, msg);
        } else
            throw cRuntimeError("Unknown Message Type: %s", packet->getName());

    } else if (msg->getKind() == TCP_I_AVAILABLE) {
        handleSocketAvailable(msg);
    } else {
        delete msg;
    }
}

/**
 * A surrogate Server wants to know where it can get the object from
 * 1. search caches to find surrogates which cache object
 * 2. decide where to direct surrogate server
 * 3. send id of where to get object from
 */
void DNSApp::handleSurrogateRequest(ISocket *arrivalSocket, cMessage *msg) {

    Packet *packet = check_and_cast<Packet*>(msg);
    const auto &appmsg = packet->peekDataAt<DNSRequest>(B(0),
            packet->getTotalLength());
    int requestedObjectId = appmsg->getObjectId();
    int requestSurrogateId = appmsg->getSurrogateId();

    vector<int> possibleSurrogates; // id's of the surrogates with the object

    msgsRcvd++;
    bytesRcvd += B(packet->getTotalLength()).get();

    // search each cache for requested object
    for (int i = 0; i < serverCaches.size(); i++) {
        unordered_set<int> serverCache = serverCaches[i];

        auto objectToFind = serverCache.find(requestedObjectId);

        if (objectToFind != serverCache.end()) {
            possibleSurrogates.push_back(i);
        }
    }

    int surrogateId = -1;

    switch (cachePolicy) {
    case 0:{
        // closest surrogate
        bool found = false;
        vector<short> otherSurrogates =
                otherSurrogatesSortedByDistance[requestSurrogateId];
        for (int i = 0; i < otherSurrogates.size(); i++) {
            int otherSurrogate = otherSurrogates[i]; // closestSurrogate := otherSurrogates[0], furthest := otherSurrogates[otherSurrogates.size()-1]

            for (int j = 0; j < possibleSurrogates.size(); j++) {
                int possibleSurrogate = possibleSurrogates[j];

                if (otherSurrogate == possibleSurrogate) {
                    // we found the closest surrogate with the cached object
                    surrogateId = otherSurrogate;
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }

        break;
    }
    case 1:
        // random surrogate
        if (possibleSurrogates.size() > 0)
            surrogateId = possibleSurrogates[intuniform(0,
                    possibleSurrogates.size() - 1)];
        break;
    case 2:{
        // load balance
        int leastLoad = 100000;
        int leastLoadIndex = -1;

        for (int i = 0; i < possibleSurrogates.size(); i++) {
            int possibleSurrogateIndex = possibleSurrogates[i];
            if (serverLoads[possibleSurrogateIndex] < leastLoad) {
                leastLoad = serverLoads[possibleSurrogateIndex];
                leastLoadIndex = possibleSurrogateIndex;
            }
        }

        surrogateId = leastLoadIndex;
        break;
    }
    case 3:
        //non coop
        surrogateId=-1;
    }

    const auto &payload = makeShared<DNSResponse>();
    Packet *outPacket = new Packet("DNSResponse");
    payload->setChunkLength(B(300)); // keep at 300; it is going to be a single packet transferred
    payload->setServerId(surrogateId);
    payload->setObjectId(requestedObjectId);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    outPacket->insertAtBack(payload);
    arrivalSocket->send(outPacket);

    msgsSent++;
    bytesSent += 300;

    delete msg;
}

/**
 *  We got a message containing the current load of a surrogate server. The message contains the index of the surrogate server and the load
 */
void DNSApp::handleLoadMessage(ISocket *arrivalSocket, cMessage *msg) {
    Packet *packet = check_and_cast<Packet*>(msg);

    const auto &payload = packet->peekDataAt<LoadDataMsg>(B(0),
            packet->getTotalLength());
    int load = payload->getLoad();
    int surrogateId = payload->getSurrogateId();
    serverLoads[surrogateId] = load;

    delete msg;
}

/**
 * A surrogate Server cache was modified
 */
void DNSApp::handleCacheModification(ISocket *arrivalSocket, cMessage *msg) {
    Packet *packet = check_and_cast<Packet*>(msg);
    const auto &appmsg = packet->peekDataAt<CacheModificationRequest>(B(0),
            packet->getTotalLength());
    int type = appmsg->getType();
    int objectId = appmsg->getObjectId();
    int surrogateIndex = appmsg->getSurrogateIndex();
    int objectToRemove = appmsg->getObjectToRemove();

    serverCaches[surrogateIndex].insert(objectId);

// remove object: send response to surrogate that it was removed here and it can remove it too
    if (objectToRemove != -1) {
        auto object = serverCaches[surrogateIndex].find(objectToRemove);
        if (object != serverCaches[surrogateIndex].end()) {
            serverCaches[surrogateIndex].erase(object);
        }

        Packet *outPacket = new Packet("CacheModificationResponse");
        const auto &payload = makeShared<CacheModificationResponse>();
        payload->setChunkLength(B(300));
        payload->setObjectToDelete(objectToRemove);
        payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
        outPacket->insertAtBack(payload);
        arrivalSocket->send(outPacket);
    }

    delete msg;
}

/**
 * Here we handle attempts to connect. Only surrogate servers will connect to the DNS server. The client redirection happens at the ClientApp.
 */
void DNSApp::handleSocketAvailable(cMessage *msg) {
    TcpAvailableInfo *availableInfo = check_and_cast<TcpAvailableInfo*>(
            msg->getControlInfo());
    cModule *module = L3AddressResolver().findHostWithAddress(
            availableInfo->getRemoteAddr());

    if (strcmp(module->getName(), "surrogateServer") == 0) {
        // we will only land in here right after initialization; the connection to surrogate servers lives forever
        TcpSocket *newSocket = new TcpSocket(availableInfo);
        newSocket->setOutputGate(gate("socketOut"));

        surrogateSocketMap.addSocket(newSocket);
        socket.accept(availableInfo->getNewSocketId());

        delete msg;

    } else {
        socket.processMessage(msg);
    }
}

/**
 * We should never get here because we are only connected to surrogate Servers and they never close connection
 */
void DNSApp::handlePeerClosed(cMessage *msg) {
    int connId =
            check_and_cast<Indication*>(msg)->getTag<SocketInd>()->getSocketId();
    delete msg;
    auto request = new Request("close", TCP_C_CLOSE);
    request->addTag<SocketReq>()->setSocketId(connId);
    sendBack(request);
}

void DNSApp::finish() {
    EV_INFO << getFullPath() << ": sent " << bytesSent << " bytes in "
                   << msgsSent << " packets\n";
    EV_INFO << getFullPath() << ": received " << bytesRcvd << " bytes in "
                   << msgsRcvd << " packets\n";
}

