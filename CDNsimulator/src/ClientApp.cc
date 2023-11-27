#include "ClientApp.h"

#include "inet/applications/tcpapp/GenericAppMsg_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/socket/SocketTag_m.h"

#include "messages/ClientRequest_m.h"
#include "messages/ServerResponse_m.h"
#include "messages/RetryMsg_m.h"

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "definitions.h"

#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace inet;

simsignal_t ClientApp::connectSignal = registerSignal("connect");
simsignal_t ClientApp::failSignal = registerSignal("sessionFail");
simsignal_t ClientApp::sendIdSignal = registerSignal("sendId");
simsignal_t ClientApp::receiveIdSignal = registerSignal("receiveId");

Define_Module(ClientApp);

ClientApp::~ClientApp() {
}

void ClientApp::initialize(int stage) {
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        numSessions  = packetsSent = packetsRcvd = bytesSent =
                bytesRcvd = 0;

        WATCH(numSessions);
        WATCH(packetsSent);
        WATCH(packetsRcvd);
        WATCH(bytesSent);
        WATCH(bytesRcvd);
    } else if (stage == INITSTAGE_APPLICATION_LAYER) {
        // parameters
        loadRequest();
        loadClientToSurrogate();
        trafficTimer = new cMessage("trafficTimer");

        if (trafficRequests.size() > 0) {
            scheduleAt( 0.5 + static_cast<double>(trafficRequests[0].timeStamp)
                                    / 1000000, trafficTimer);
        }

    }
}

/**
 *  Load trace file
 */
void ClientApp::loadRequest() {
    int thisIndex = getParentModule()->getIndex();
    string path = string(par("locationOfTrafficFolder").stringValue())
            + "/client" + to_string(thisIndex) + "requests.txt";

    ifstream inputFile(path.c_str());
    if (!inputFile.is_open())
        throw cRuntimeError("Traffic file did not open");

    string line;
    while (getline(inputFile, line)) {
        istringstream iss(line);

        long timeStamp, clientId, objectId, requestId;
        if (!(iss >> timeStamp >> clientId >> objectId >> requestId)) {
            continue;
        }

        trafficRequests.push_back(
                TrafficRequest(requestId, clientId, objectId, timeStamp));
    }
    inputFile.close();
}

/**
 * load file where for each clientId a surrogateId is given
 */
void ClientApp::loadClientToSurrogate() {
    int thisIndex = getParentModule()->getIndex();
    string path = string(par("locationOfClientToSurrogateFolder").stringValue())
            + "/client" + to_string(thisIndex) + "toSurrogate.txt";

    ifstream inputFile(path.c_str());
    if (!inputFile.is_open())
        throw cRuntimeError("ClientToSurrogate file did not open");

    string line;
    while (getline(inputFile, line)) {
        istringstream iss(line);

        int clientId, surrogateId;
        if (!(iss >> clientId >> surrogateId)) {
            continue;
        }

        clientToSurrogate.insert( { clientId, surrogateId });
    }
    inputFile.close();
}

void ClientApp::handleStartOperation(LifecycleOperation *operation) {

}

void ClientApp::handleStopOperation(LifecycleOperation *operation) {
    for (auto const &x : socketMap.getMap()) {
        TcpSocket *socket = check_and_cast<TcpSocket*>(x.second);
        if (socket->getState() == TcpSocket::CONNECTED
                || socket->getState() == TcpSocket::CONNECTING
                || socket->getState() == TcpSocket::PEER_CLOSED) {
            x.second->close();
        }
    }
}

void ClientApp::handleCrashOperation(LifecycleOperation *operation) {
    if (operation->getRootModule() != getContainingNode(this)) {
        for (auto const &x : socketMap.getMap()) {
            x.second->destroy();
        }
        socketMap.deleteSockets();
    }
}

/*
* Handle requests
*/
void ClientApp::handleMessageWhenUp(cMessage *msg) {
    if (msg->isSelfMessage()) {
        if (dynamic_cast<RetryMsg*>(msg) != nullptr) {
            RetryMsg *retryMsg = check_and_cast<RetryMsg*>(msg);
            handleRetry(retryMsg->getConnId());
            delete msg;
        } else if (strcmp(trafficTimer->getName(), "trafficTimer") == 0) {
            handleTrafficMsg();
        }
    } else {
        // handle server response
        ISocket *socket = socketMap.findSocketFor(msg);
        socket->processMessage(msg);
    }
}

/*
* handle self messages to send out request
*/
void ClientApp::handleTrafficMsg() {
    TrafficRequest trafficRequest = trafficRequests[0];

    long objectId = trafficRequest.objectId;
    long clientId = trafficRequest.clientId;
    long requestId = trafficRequest.requestId;

    int connId = connect(clientId);

    RetryObject retryObject(objectId, clientId, requestId);

    retryMap.insert( { connId, retryObject });

    trafficRequests.erase(trafficRequests.begin());
    if (trafficRequests.size() > 0) {
        scheduleAt(0.5 + static_cast<double>(trafficRequests[0].timeStamp) / 1000000,
                trafficTimer);
    }
}

/*
* connect to surrogate server
*/
int ClientApp::connect(long clientId) {
    //  we need a new socket
    TcpSocket *newSocket = new TcpSocket();
    newSocket->bind(L3Address(), -1);

    newSocket->setCallback(this);
    newSocket->setOutputGate(gate("socketOut"));
    socketMap.addSocket(newSocket);

    // connect
    string connectAddress = "surrogateServer["
            + to_string(clientToSurrogate[clientId]) + "]";

    L3Address destination;
    L3AddressResolver().tryResolve(connectAddress.c_str(), destination);
    if (destination.isUnspecified()) {
        EV_ERROR << "Connecting to " << connectAddress << " port=" << 1000
                        << ": cannot resolve destination address\n";
    } else {
        newSocket->connect(destination, 1000);

        numSessions++;
        emit(connectSignal, 1L);
    }

    return newSocket->getSocketId();
}
 
/*
* three way handshake succeded
*/
void ClientApp::socketEstablished(TcpSocket *socket) {
    int connId = socket->getSocketId();

    auto it = retryMap.find(connId);
    if (it == retryMap.end())
        throw cRuntimeError("retryObject does not exist");

    RetryObject retryObject = it->second;

    int objectId = retryObject.objectId;
    long requestId = retryObject.requestId;
    sendRequest(objectId, connId, requestId);

}

/*
* send request to surrogate server
*/
void ClientApp::sendRequest(int objectId, int connId, long requestId) {
    const auto &payload = makeShared<ClientRequest>();
    Packet *packet = new Packet("data");
    payload->setChunkLength(B(300));
    payload->setObjectId(objectId);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);

    int numBytes = packet->getByteLength();
    emit(packetSentSignal, packet);

    TcpSocket *socket = check_and_cast_nullable<TcpSocket*>(
            socketMap.getSocketById(connId));

    socket->send(packet);

    emit(sendIdSignal, requestId);

    packetsSent++;
    bytesSent += numBytes;
}

/*
* response from surrogate server arrived
*/
void ClientApp::socketDataArrived(TcpSocket *socket, Packet *msg, bool) {

    int connId = msg->getTag<SocketInd>()->getSocketId();
    ChunkQueue &queue = socketQueue[connId];
    auto chunk = msg->peekDataAt(B(0), msg->getTotalLength());
    queue.push(chunk);

    while (queue.has<ServerResponse>(b(-1))) {

        const auto &appMsg = queue.pop<ServerResponse>(b(-1));

        short statusCode = appMsg->getStatusCode();
        int requestedObjectId = appMsg->getRequestedObjectId();
        long size = appMsg->getSize();

        packetsRcvd++;
        bytesRcvd += B(appMsg->getChunkLength()).get();

        auto it = retryMap.find(connId);
        if (it == retryMap.end())
            throw cRuntimeError("retryObject does not exist");
        RetryObject retryObject = it->second;

        emit(packetReceivedSignal, msg);
        emit(receiveIdSignal, retryObject.requestId);

        if (it != retryMap.end()) {
            retryMap.erase(it);
        }

        auto it1 = socketQueue.find(connId);
        // socketQueue.erase(it1);
        // break;

    }


    delete msg;
}

/*
* surrogate server is full and it denied connection
*/
void ClientApp::socketFailure(TcpSocket *socket, int code) {
    // subclasses may override this function, and add code try to reconnect after a delay.
    packetsSent--;
    bytesSent -= 300;

    RetryMsg *retryMsg = new RetryMsg("retryMsg");
    retryMsg->setConnId(socket->getSocketId());

    int connId = socket->getSocketId();

    // fail sessions statistic
    auto it = retryMap.find(connId);
    if (it == retryMap.end())
        throw cRuntimeError("SocketFailure: retryObject does not exist");
    RetryObject retryObject = it->second;


    scheduleAt(simTime() + intuniform(10,20), retryMsg);

    emit(failSignal, retryObject.requestId);
    emit(connectSignal, -1L);
    }

/*
* retry the request
*/
void ClientApp::handleRetry(int connId) {

    auto it = retryMap.find(connId);
    if (it == retryMap.end())
        throw cRuntimeError("HandleRetry: retryObject does not exist");
    RetryObject retryObject = it->second;

    int objectId = retryObject.objectId;
    long requestId = retryObject.requestId;

    if (it != retryMap.end()) {
        retryMap.erase(it);
    }

    int newConnId = connect(retryObject.clientId);

    retryMap.insert( { newConnId, retryObject });

}

/**
 * peer closed so we are closing too
 */
void ClientApp::socketPeerClosed(TcpSocket *socket) {
    if (socket->getState() == TcpSocket::PEER_CLOSED) {
        socket->close();
        emit(connectSignal, -1L);
    }
}

/*
* remove socket from socketmap when closed
*/
void ClientApp::socketClosed(TcpSocket *socket) {
    // delete the Socket from map
    ISocket *deletedSocket = socketMap.removeSocket(socket);
    if (deletedSocket == nullptr)
        EV_ERROR << "Trying to delete socket which does not exists in map";
}

void ClientApp::socketAvailable(TcpSocket *socket,
        TcpAvailableInfo *availableInfo) {
}

void ClientApp::socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status) {
}

void ClientApp::refreshDisplay() const {
    ApplicationBase::refreshDisplay();
}

void ClientApp::finish() {
    string modulePath = getFullPath();

    EV_INFO << modulePath << ": opened " << numSessions << " sessions\n";
    EV_INFO << modulePath << ": sent " << bytesSent << " bytes in "
                   << packetsSent << " packets\n";
    EV_INFO << modulePath << ": received " << bytesRcvd << " bytes in "
                   << packetsRcvd << " packets\n";
}
