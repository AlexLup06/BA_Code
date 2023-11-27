#include "OriginServerApp.h"

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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <string>

#include "messages/ObjectMsg_m.h"
#include "messages/ObjectRequest_m.h"
#include "messages/ObjectResponse_m.h"

using namespace inet;

Define_Module(OriginServerApp);
simsignal_t OriginServerApp::loadInfoOriginSignal = registerSignal(
        "loadInfoOrigin");
simsignal_t OriginServerApp::originRequestInSignal = registerSignal("originRequestIn");
simsignal_t OriginServerApp::originResponseOutSignal = registerSignal("originResponseOut");

void OriginServerApp::initialize(int stage) {
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {

        // statistics
        msgsRcvd = msgsSent = bytesRcvd = bytesSent = 0;

        WATCH(msgsRcvd);
        WATCH(msgsSent);
        WATCH(bytesRcvd);
        WATCH(bytesSent);

    } else if (stage == INITSTAGE_APPLICATION_LAYER) {
        int localPort = 1000;
        socket.setOutputGate(gate("socketOut"));
        socket.bind(L3Address(), localPort);
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
        loadObjects();  // load web objects
        createTcpConnectionToSurrogates();

        computeTime = par("computeTime").doubleValue();
        sendLoadInterval = par("sendLoadInterval").doubleValue();

        computeTimer = new cMessage("computeTimer");
        loadTimer = new cMessage("loadTimer");
        scheduleAt(simTime() + 0.1, loadTimer);
    }
}

/*
 * load the object from the objects file
 */
void OriginServerApp::loadObjects() {
    ifstream inputFile(string(par("pathToObjectsFile").stringValue()).c_str());
    if (!inputFile.is_open())
        throw cRuntimeError("Wrong path to objectFile\n");

    string line;
    while (getline(inputFile, line)) {
        istringstream iss(line);
        long id, objectSize;
        if (!(iss >> id >> objectSize)) {
            continue;
        }
        objects.push_back(CacheObject(id, objectSize));
    }
    inputFile.close();
}

/*
 * ConnectPorts:
 * 1001, 1002, ... (1001 + numSurrogates - 1) for incoming/outgoing requests
 */
void OriginServerApp::createTcpConnectionToSurrogates() {
    numSurrogates = getSystemModule()->par("numSurrogateServers");

    for (int i = 0; i < numSurrogates; i++) {
        //  we need a new socket for every surrogate server
        string connectAddress = "surrogateServer[" + to_string(i) + "]";

        TcpSocket *newSocket = new TcpSocket();
        newSocket->bind(L3Address(), 1001 + i);
        newSocket->setOutputGate(gate("socketOut"));
        L3Address destination;
        L3AddressResolver().tryResolve(connectAddress.c_str(), destination);
        newSocket->connect(destination, 1000);
        surrogateSocketMap.addSocket(newSocket);
    }
}

/*
* send packet
*/
void OriginServerApp::sendBack(cMessage *msg) {
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

/*
 * handle all messages that arrive here
 */
void OriginServerApp::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        if (strcmp(msg->getName(), "computeTimer") == 0) {
            handleComputeTimer();
        } else if (strcmp(msg->getName(), "loadTimer") == 0) {
            handleLoadTimer();
        } else {
            sendBack(msg);
        }

    } else if (msg->getKind() == TCP_I_PEER_CLOSED) {
        handlePeerClosed(msg);
    } else if (msg->getKind() == TCP_I_DATA
            || msg->getKind() == TCP_I_URGENT_DATA) {

        ISocket *arrivalSocket = surrogateSocketMap.findSocketFor(msg);

        if (arrivalSocket == nullptr)
            throw cRuntimeError("Got message from unknown node");

        // send requested object to surrogate server
        handleSurrogateRequest(arrivalSocket, msg);
    } else if (msg->getKind() == TCP_I_AVAILABLE)
        socket.processMessage(msg);
    else {
        delete msg;
    }
}

/**
 * load statistics
 */
void OriginServerApp::handleLoadTimer() {
    emit(loadInfoOriginSignal, queue.size());
    scheduleAt(simTime() + sendLoadInterval, loadTimer);
}

/**
 * We receive a request from a surrogate server for an object
 */
void OriginServerApp::handleSurrogateRequest(ISocket *arrivalSocket,
        cMessage *msg) {


    Packet *packet = check_and_cast<Packet*>(msg);
    int connId = packet->getTag<SocketInd>()->getSocketId();
    ChunkQueue &queueChunk = socketQueue[connId];
    auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
    queueChunk.push(chunk);
    while (queueChunk.has<ObjectRequest>(b(-1))) {
        emit(originRequestInSignal,1);
        const auto &appmsg = queueChunk.pop<ObjectRequest>(b(-1));
        int requestedObjectId = appmsg->getObjectId();

        if (queue.size() == 0)
            scheduleAt(simTime() + computeTime, computeTimer);

        queue.push_back(
                QueueObject(arrivalSocket->getSocketId(), requestedObjectId));

        msgsRcvd++;
        bytesRcvd += B(appmsg->getChunkLength()).get();
    }
    delete msg;
}

/**
 *  compute object request
 */
void OriginServerApp::handleComputeTimer() {

    QueueObject request = queue[0];

    int connId = request.connId;
    int objectId = request.objectId;

    int foundIndex = -1;
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i].id == objectId) {
            foundIndex = i;
            break;
        }
    }

    int size = objects[foundIndex].size;

    if (foundIndex == -1)
        throw cRuntimeError("No Object with Id: %d", objectId);

    Packet *outPacket = new Packet("data", TCP_C_SEND);
    outPacket->addTag<SocketReq>()->setSocketId(connId);
    const auto &payload = makeShared<MyObjectResponse>();
    payload->setChunkLength(B(size));
    payload->setObjectId(objectId);
    payload->setSize(size);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    outPacket->insertAtBack(payload);
    surrogateSocketMap.getSocketById(connId)->send(outPacket);

    emit(originResponseOutSignal,1);

    msgsSent++;
    bytesSent += B(objects[foundIndex].size).get();

    queue.erase(queue.begin());
    if (queue.size() > 0)
        scheduleAt(simTime() + computeTime, computeTimer);

}
/**
 *  We should never arrive here because we are only connected to surrogate servers and they never close connection
 */
void OriginServerApp::handlePeerClosed(cMessage *msg) {
    int connId =
            check_and_cast<Indication*>(msg)->getTag<SocketInd>()->getSocketId();
    delete msg;
    auto request = new Request("close", TCP_C_CLOSE);
    request->addTag<SocketReq>()->setSocketId(connId);
    sendBack(request);
}

void OriginServerApp::finish() {
    if (computeTimer->isScheduled())
          cancelEvent(computeTimer);
   delete computeTimer;
   if (loadTimer->isScheduled())
        cancelEvent(loadTimer);
   delete loadTimer;
    EV_INFO << getFullPath() << ": sent " << bytesSent << " bytes in "
                   << msgsSent << " packets\n";
    EV_INFO << getFullPath() << ": received " << bytesRcvd << " bytes in "
                   << msgsRcvd << " packets\n";
}
