#include "SurrogateServerApp.h"

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
#include "messages/ClientRequest_m.h"
#include "messages/ServerResponse_m.h"
#include "messages/LoadDataMsg_m.h"
#include "messages/DNSResponse_m.h"
#include "messages/DNSRequest_m.h"
#include "messages/ObjectRequest_m.h"
#include "messages/ObjectResponse_m.h"
#include "messages/CacheModificationRequest_m.h"
#include "messages/CacheModificationResponse_m.h"
#include "messages/FinishMsg_m.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <string>

using namespace inet;

Define_Module(SurrogateServerApp);

simsignal_t SurrogateServerApp::surrogateLoad = registerSignal("loadSurrogate");
simsignal_t SurrogateServerApp::surrogateLoadObjects = registerSignal("surrogateLoadObjects");

simsignal_t SurrogateServerApp::surrogateObjectRequestInSignal = registerSignal("surrogateObjectRequestIn");
simsignal_t SurrogateServerApp::surrogateObjectResponseOutSignal = registerSignal("surrogateObjectResponseOut");
simsignal_t SurrogateServerApp::surrogateClientRequestInSignal = registerSignal("surrogateClientRequestIn");
simsignal_t SurrogateServerApp::surrogateClientResponseOutSignal = registerSignal("surrogateClientResponseOut");

void SurrogateServerApp::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL)
    {

        // statistics
        msgsRcvd = msgsSent = bytesRcvd = bytesSent = 0;

        WATCH(msgsRcvd);
        WATCH(msgsSent);
        WATCH(bytesRcvd);
        WATCH(bytesSent);
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        int localPort = 1000;
        socket.setOutputGate(gate("socketOut"));
        socket.bind(L3Address(), localPort);
        socket.listen();

        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus =
            node ? check_and_cast_nullable<NodeStatus *>(
                       node->getSubmodule("status"))
                 : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError(
                "This module doesn't support starting in node DOWN state");
    }
    else if (stage == INITSTAGE_LAST)
    {
        // connection to origin server is initialized from origin server
        cModule *parentMod = getParentModule();
        thisIndex = parentMod->getIndex();
        numSurrogates = parentMod->getVectorSize();

        // parameters
        cachePolicy = getSystemModule()->par("cachePolicy").intValue();
        computeTime = par("computeTime").doubleValue();
        maxCacheSize = par("maxCacheSize").intValue(); // 10% of numbers of objects
        MAX_NUMBER_CONNECTIONS = par("maxNumberConnections").intValue();
        sendLoadInterval = par("sendLoadInterval").doubleValue();
        priorityTime = par("priorityTime").doubleValue();

        // timers
        loadTimer = new cMessage("loadTimer");
        scheduleAt(simTime() + 0.1, loadTimer);
        computeTimer = new cMessage("computeTimer");
        priorityTimer = new cMessage("priorityTimer");
        objectRequestTimer = new cMessage("objectRequestTimer");

        cache = new LRUCache(maxCacheSize);

        connectToSurrogates();
        connectToDNS();

    }
}

/*
 *  establish a connection to DNS server
 */
void SurrogateServerApp::connectToDNS()
{
    //  we need a new socket for every surrogate server
    dnsSocket = new TcpSocket();
    dnsSocket->bind(L3Address(), 999);
    dnsSocket->setOutputGate(gate("socketOut"));

    // connect
    string connectAddress = "dnsServer";
    int connectPort = 1000 + thisIndex + 1;

    L3Address destination;
    L3AddressResolver().tryResolve(connectAddress.c_str(), destination);

    dnsSocket->connect(destination, 1000);
}

/*
 * connect to every other surrogate server which we have no connection to.
 * 0 connects to 1,2,3,...
 * 1 connects to 2,3,4,...
 * 2 connects to 3,4,5,...
 *
 *  Ports:
 *  999 for dns
 *  1000 for Clients
 *  1001 for Origin
 *  1002, 1003, ... (1002 + numSurrogates - 1) for incoming/outgoing requests from Surrogates
 *  (1002 + numSurrogates), (1002+numSurrogates+1), ... (1002 + 2*numSurrogates - 1) for incoming/outgoing responses to Surrogates
 */
void SurrogateServerApp::connectToSurrogates()
{

    for (int i = thisIndex + 1; i < numSurrogates; i++)
    {
        string connectAddress = "surrogateServer[" + to_string(i) + "]";

        TcpSocket *requestSocket = new TcpSocket();
        requestSocket->bind(L3Address(), 1002);
        requestSocket->setOutputGate(gate("socketOut"));

        TcpSocket *responseSocket = new TcpSocket();
        responseSocket->bind(L3Address(), 1002 + numSurrogates);
        responseSocket->setOutputGate(gate("socketOut"));

        surrogateSocketRequestMap.addSocket(requestSocket);
        surrogateSocketResponseMap.addSocket(responseSocket);

        L3Address destination;
        L3AddressResolver().tryResolve(connectAddress.c_str(), destination);

        requestSocket->connect(destination, 1000);
        responseSocket->connect(destination, 1000);

        indexIdRequestMap.insert({i, requestSocket->getSocketId()});
        indexIdResponseMap.insert({i, responseSocket->getSocketId()});
    }
}

/*
* send packet
*/
void SurrogateServerApp::sendBack(cMessage *msg)
{
    Packet *packet = dynamic_cast<Packet *>(msg);

    if (packet)
    {
        msgsSent++;
        bytesSent += packet->getByteLength();
        emit(packetSentSignal, packet);
    }

    auto &tags = check_and_cast<ITaggedObject *>(msg)->getTags();
    tags.addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::tcp);
    send(msg, "socketOut");
}

/*
 * Handle all kinds of msg
 *
 * 1. Self messages
 * 2. Peer closed
 * 3. TCP data from origin, DNS, other surrogates and clients
 * 4. Connection requests
 * 5. Unkown Messages
 */
void SurrogateServerApp::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        if (strcmp(msg->getName(), "loadTimer") == 0)
        {
            handleLoadTimer();
        }
        else if (strcmp(msg->getName(), "computeTimer") == 0)
        {
            handleComputeTimer();
        }
        else if (strcmp(msg->getName(), "priorityTimer") == 0)
        {
            handlePriorityTimer();
        }
        else if (strcmp(msg->getName(), "objectRequestTimer") == 0)
        {
            handleObjectRequestTimer();
        }
        else
        {
            sendBack(msg);
        }
    }
    else if (msg->getKind() == TCP_I_PEER_CLOSED)
    {
        handlePeerClosed(msg);
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA)
    {

        // msgs from surrogates or origin can only be responses containing an object upon requesting an object if a cache miss occurred
        ISocket *arrivalSocketRequest = surrogateSocketRequestMap.findSocketFor(
            msg);
        ISocket *arrivalSocketResponse =
            surrogateSocketResponseMap.findSocketFor(msg);

        if (originSocket->belongsToSocket(msg))
        {
            handleObjectResponse(msg);
            return;
        }
        else if (arrivalSocketRequest != nullptr && arrivalSocketResponse == nullptr)
        {
            handleObjectRequest(arrivalSocketRequest, msg);
            return;
        }
        else if (arrivalSocketRequest == nullptr && arrivalSocketResponse != nullptr)
        {
            handleObjectResponse(msg);
            return;
        }

        Packet *packet = check_and_cast<Packet *>(msg);
        const char *msgType = packet->peekAtFront()->getClassName();
        // responses from DNS are msgs which tell the surrogate server from which surrogate server to get object from
        if (dnsSocket->belongsToSocket(msg))
        {
            if (strcmp(msgType, "inet::CacheModificationResponse") == 0)
            {
                handleCacheModificationResponse(msg);
            }
            else
            {
                handleDNSResponse(msg);
            }
            return;
        }

        // all other requests are client requests
        handleClientRequest(msg);
    }
    else if (msg->getKind() == TCP_I_AVAILABLE)
    {
        handleSocketAvailable(msg);
    }
    else
    {
        delete msg;
    }
}

/*
 * Send the current load to DNS server
 */
void SurrogateServerApp::handleLoadTimer()
{
    emit(surrogateLoad, currentConnections);
    emit(surrogateLoadObjects, objectRequestQueue.size());
    const auto &payload = makeShared<LoadDataMsg>();
    Packet *packet = new Packet("loadData");
    payload->setChunkLength(B(300));
    payload->setLoad(objectRequestQueue.size());
    payload->setSurrogateId(thisIndex);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);
    dnsSocket->send(packet);

    scheduleAt(simTime() + 0.1, loadTimer);
}


/*
 * Add request from client to FIFO queue
 */
void SurrogateServerApp::handleClientRequest(cMessage *msg)
{

    emit(surrogateClientRequestInSignal, 1);
    cMessage *dupMsg = msg->dup();
    Packet *packet = check_and_cast<Packet *>(dupMsg);
    int connId = packet->getTag<SocketInd>()->getSocketId();

    if (queue.size() == 0)
        scheduleAt(simTime() + computeTime, computeTimer);
    queue.push_back(packet);

    emit(packetReceivedSignal, packet);

    delete msg;
}

/*
 * Compute client request. FIFO queue
 *
 * case 1: we have the object in cache and can send it out right away
 * case 2: we do not have object in cache
 *  a: the object was already requested -> just append the connId to the cacheMisses
 *  b: the object was not requested -> request the object and create an entry to cacheMisses
 */
void SurrogateServerApp::handleComputeTimer()
{

    Packet *packet = queue[0];
    int connId = packet->getTag<SocketInd>()->getSocketId(); // unique connection Id of a client

    const auto &appmsg = packet->peekDataAt<ClientRequest>(B(0),
                                                           packet->getTotalLength());
    int requestedObjectId = appmsg->getObjectId();

    msgsRcvd++;
    bytesRcvd += B(appmsg->getChunkLength()).get();

    long objectSize = cache->get(requestedObjectId);

    // case 2
    if (objectSize == -1)
    {

        auto it = cacheMisses.find(requestedObjectId);
        if (it != cacheMisses.end())
        {
            // case 2a
            it->second.push_back(connId);
        }
        else
        {
            // case 2b
            handleCacheMiss(connId, requestedObjectId); // only send request when not already sent
            vector<int> cacheMissVector = {connId};
            cacheMisses.insert({requestedObjectId, cacheMissVector});
        }

        queue.erase(queue.begin());
        if (queue.size() > 0)
            scheduleAt(simTime() + computeTime, computeTimer);
        return;
    }

    // case 1

    Packet *outPacket = new Packet(packet->getName(), TCP_C_SEND);
    outPacket->addTag<SocketReq>()->setSocketId(connId);
    const auto &payload = makeShared<ServerResponse>();

    payload->setChunkLength(B(objectSize));
    payload->setRequestedObjectId(requestedObjectId);
    payload->setSize(objectSize);
    payload->setStatusCode(200);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    outPacket->insertAtBack(payload);
    sendBack(outPacket);

    emit(surrogateClientResponseOutSignal, 1);

    // server closes connection right away
    auto request = new Request("close", TCP_C_CLOSE);
    TcpCommand *cmd = new TcpCommand();
    request->addTag<SocketReq>()->setSocketId(connId);
    request->setControlInfo(cmd);
    sendBack(request);

    currentConnections--;

    queue.erase(queue.begin());
    if (queue.size() > 0)
    {
        scheduleAt(simTime() + computeTime, computeTimer);
    }
}

/*
 *  the priorityQueue are the cache misses. They are treated seperatly in a second "thread"
 */
void SurrogateServerApp::handlePriorityTimer()
{

    int objectToSend = priorityQueue[0];
    long objectSize = cache->get(objectToSend);
    vector<int> connIds = cacheMisses[objectToSend];

    for (int i = 0; i < connIds.size(); i++)
    {
        Packet *outPacket = new Packet("data", TCP_C_SEND);
        outPacket->addTag<SocketReq>()->setSocketId(connIds[i]);
        const auto &payload = makeShared<ServerResponse>();

        payload->setChunkLength(B(objectSize));
        payload->setRequestedObjectId(objectToSend);
        payload->setStatusCode(200);
        payload->setSize(objectSize);
        payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
        outPacket->insertAtBack(payload);
        sendBack(outPacket);

        emit(surrogateClientResponseOutSignal,1);

        // server closes connection right away
        auto request = new Request("close", TCP_C_CLOSE);
        TcpCommand *cmd = new TcpCommand();
        request->addTag<SocketReq>()->setSocketId(connIds[i]);
        request->setControlInfo(cmd);
        sendBack(request);

        currentConnections--;
    }

    priorityQueue.erase(priorityQueue.begin());
    cacheMisses.erase(objectToSend);

    if (priorityQueue.size() > 0)
    {
        scheduleAt(simTime() + priorityTime, priorityTimer);
    }
}

/*
 *  Send request to DNS to get info where to get object from or request it from origin server if not cooperative
 */
void SurrogateServerApp::handleCacheMiss(int connId, int requestedObjectId)
{

    Packet *packet;

    // cooperative -> request DNS to find out which surrogate to get object from
    const auto &payload = makeShared<DNSRequest>();
    packet = new Packet("dnsrequest");
    payload->setChunkLength(B(300)); // keep at 300; it is going to be a single packet transferred
    payload->setObjectId(requestedObjectId);
    payload->setConnId(connId);
    payload->setSurrogateId(thisIndex);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);
    dnsSocket->send(packet);
}

/*
 * Request object either from surrogate server or origin server
 * If serverId==-1 then request from origin server, otheriwse from the specified serverId;
 * CAUTION: serverId is index of the surrogateServer -> need to get socketId from indexIdRequestMap
 */
void SurrogateServerApp::handleDNSResponse(cMessage *msg)
{
    Packet *packet = check_and_cast<Packet *>(msg);
    const auto &appmsg = packet->peekDataAt<DNSResponse>(B(0),
                                                         packet->getTotalLength());
    int serverId = appmsg->getServerId();
    int requestedObjectId = appmsg->getObjectId();

    // send request
    Packet *outPacket = new Packet("objreq");
    const auto &payload = makeShared<ObjectRequest>();
    payload->setChunkLength(B(300));
    payload->setObjectId(requestedObjectId);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    outPacket->insertAtBack(payload);

    if (serverId == -1)
    {
        originSocket->send(outPacket);
    }
    else
    {
        int socketId = indexIdRequestMap[serverId];
        TcpSocket *sendSocket = check_and_cast<TcpSocket *>(
            surrogateSocketRequestMap.getSocketById(socketId));
        sendSocket->send(outPacket);
    }
    delete msg;
}

/*
 * A surrogate server requested an object. Queue the request
 */
void SurrogateServerApp::handleObjectRequest(ISocket *arrivalSocket,
                                             cMessage *msg)
{

    Packet *packet = check_and_cast<Packet *>(msg);
    int connId = packet->getTag<SocketInd>()->getSocketId();
    ChunkQueue &chunkQueue = socketQueue[connId];
    auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
    chunkQueue.push(chunk);

    while (chunkQueue.has<ObjectRequest>(b(-1)))
    {
        emit(surrogateObjectRequestInSignal, 1);
        requestsFromSurrogates++;
        const auto &appmsg = chunkQueue.pop<ObjectRequest>(b(-1));

        int requestedObjectId = appmsg->getObjectId();
        int objectSize = cache->get(requestedObjectId);

        int requestIndex = 0;
        for (const auto &pair : indexIdRequestMap)
        {
            if (pair.second == connId)
            {
                requestIndex = pair.first;
                break;
            }
        }
        int responseSocketId = indexIdResponseMap[requestIndex];

        if (objectSize == -1)
        {
            const auto &payload = makeShared<MyObjectResponse>();
            Packet *outPacket = new Packet(
                string("objres" + to_string(requestedObjectId)).c_str());
            payload->setChunkLength(B(300));
            payload->setObjectId(requestedObjectId);
            payload->setSize(-1);
            payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
            outPacket->insertAtBack(payload);
            surrogateSocketResponseMap.getSocketById(responseSocketId)->send(outPacket);
            delete msg;
            return;
        }
        if (objectRequestQueue.size() == 0)
        {
            scheduleAt(simTime() + computeTime, objectRequestTimer);
        }

        objectRequestQueue.push_back(
            ObjectRequestObject(objectSize, requestedObjectId,
                                responseSocketId));
    }
    delete msg;
}

/*
 *  Handle queded requests for object from surrogate servers
 */
void SurrogateServerApp::handleObjectRequestTimer()
{
    ObjectRequestObject objectRequest = objectRequestQueue[0];

    // send the requested object to the surrogate server
    Packet *outPacket = new Packet(
        string("ObjectResponse" + to_string(objectRequest.connId)).c_str());
    const auto &payload = makeShared<MyObjectResponse>();
    outPacket->addTag<SocketReq>()->setSocketId(objectRequest.connId);
    payload->setChunkLength(B(objectRequest.objectSize));
    payload->setObjectId(objectRequest.requestedObjectId);
    payload->setSize(objectRequest.objectSize);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    outPacket->insertAtBack(payload);

    ISocket *arrivalSocket = surrogateSocketResponseMap.getSocketById(
        objectRequest.connId);
    arrivalSocket->send(outPacket);
    emit(surrogateObjectResponseOutSignal, 1);

    objectRequestQueue.erase(objectRequestQueue.begin());
    if (objectRequestQueue.size() > 0)
    {
        scheduleAt(simTime() + computeTime, objectRequestTimer);
    }
}

/*
 * Got the requested object. Send it to client and tell dns server about cache modification
 * Sometimes we don't get it and need to request from origin
 */
void SurrogateServerApp::handleObjectResponse(cMessage *msg)
{
    Packet *packet = check_and_cast<Packet *>(msg);
    int connId = packet->getTag<SocketInd>()->getSocketId();
    ChunkQueue &chunkQueue = socketQueue[connId];
    auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
    chunkQueue.push(chunk);

    while (chunkQueue.has<MyObjectResponse>(b(-1)))
    {
        const auto &appmsg = chunkQueue.pop<MyObjectResponse>(b(-1));
        int requestedObjectId = appmsg->getObjectId();
        long requestedObjectSize = appmsg->getSize();

        if (requestedObjectSize == -1)
        {
            // case: surrogate server did not have the object. Ask for it origin

            Packet *outPacket1 = new Packet("object");
            const auto &payload1 = makeShared<ObjectRequest>();
            payload1->setChunkLength(B(300));
            payload1->setObjectId(requestedObjectId);
            payload1->addTag<CreationTimeTag>()->setCreationTime(simTime());
            outPacket1->insertAtBack(payload1);

            originSocket->send(outPacket1);
        }
        else
        {
            // case: surrogate/origin had the object

            int objectToRemove = cache->set(requestedObjectId,
                                            requestedObjectSize); // -1 if NO REMOVING should happen

            // send a modificationRequest to DNS
            Packet *outPacket = new Packet("CacheModificationRequest");
            const auto &payload = makeShared<CacheModificationRequest>();
            payload->setChunkLength(B(300));
            payload->setObjectId(requestedObjectId);
            payload->setType(1);
            payload->setSurrogateIndex(thisIndex);
            payload->setObjectToRemove(objectToRemove);
            payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
            outPacket->insertAtBack(payload);
            dnsSocket->send(outPacket);

            // send object to all clients waiting for this object -> add to priorityQueue
            // only compute right away if no other computing right now
            if (priorityQueue.size() == 0)
                scheduleAt(simTime() + priorityTime, priorityTimer);
            priorityQueue.push_back(requestedObjectId);
        }
    }

    delete msg;
}

/*
 * Remove object from cache
 */
void SurrogateServerApp::handleCacheModificationResponse(cMessage *msg)
{
    Packet *packet = check_and_cast<Packet *>(msg);
    const auto &appmsg = packet->peekDataAt<CacheModificationResponse>(B(0),
                                                                       packet->getTotalLength());
    int objectIdToRemove = appmsg->getObjectToDelete();
    cache->remove(objectIdToRemove);
    delete msg;
}

/*
 *  Accept connections to surrogate, origin and client. Decline client if maximum number of connections is reached
 *
 *  Ports:
 *  999 for dns
 *  1000 for Clients
 *  1001 for Origin
 *  1002, 1003, ... (1002 + numSurrogates - 1) for incoming/outgoing  requests from Surrogates
 *  (1002 + numSurrogates), (1002+numSurrogates+1), ... (1002 + 2*numSurrogates - 1) for incoming/outgoing responses to Surrogates
 */
void SurrogateServerApp::handleSocketAvailable(cMessage *msg)
{

    TcpAvailableInfo *availableInfo = check_and_cast<TcpAvailableInfo *>(
        msg->getControlInfo());
    cModule *module = L3AddressResolver().findHostWithAddress(
        availableInfo->getRemoteAddr());

    if (strcmp(module->getName(), "surrogateServer") == 0)
    {

        int remotePort = availableInfo->getRemotePort();


        if (remotePort >= 1002 && remotePort <= 1002 + numSurrogates - 1)
        {
            // send/receive REQUESTS from surrogates
            TcpSocket *requestSocket = new TcpSocket(availableInfo);
            requestSocket->setOutputGate(gate("socketOut"));
            surrogateSocketRequestMap.addSocket(requestSocket);
            socket.accept(availableInfo->getNewSocketId());

            // needed to map index of server to socketId to request object
            indexIdRequestMap.insert(
                {module->getIndex(), requestSocket->getSocketId()});
        }
        else if (remotePort >= 1002 + numSurrogates && remotePort <= 1002 + 2 * numSurrogates - 1)
        {
            // send/receive RESPONSES to surrogates
            TcpSocket *responseSocket = new TcpSocket(availableInfo);
            responseSocket->setOutputGate(gate("socketOut"));
            surrogateSocketResponseMap.addSocket(responseSocket);
            socket.accept(availableInfo->getNewSocketId());

            indexIdResponseMap.insert(
                {module->getIndex(), responseSocket->getSocketId()});
        }
        else
        {
            throw cRuntimeError("Unknown remote surrogate server port");
        }

        delete msg;
        return;
    }

    if (strcmp(module->getName(), "originServer") == 0)
    {
        // we will only land in here right after initialization; the connection between surrogate servers and origin lives forever
        int remotePort = availableInfo->getRemotePort();

        if (remotePort == 1001 + thisIndex)
        {
            originSocket = new TcpSocket(availableInfo);
            originSocket->setOutputGate(gate("socketOut"));
            socket.accept(availableInfo->getNewSocketId());
        }
        else
        {
            throw cRuntimeError("Unknown remote origin server port");
        }

        delete msg;
        return;
    }

    if (currentConnections >= MAX_NUMBER_CONNECTIONS)
    {
        // decline connection
        auto request = new Request("abort", TCP_C_ABORT);
        availableInfo = check_and_cast<TcpAvailableInfo *>(
            msg->getControlInfo());
        TcpCommand *cmd = new TcpCommand();
        request->addTag<SocketReq>()->setSocketId(
            availableInfo->getNewSocketId());
        request->setControlInfo(cmd);
        sendBack(request);
        delete msg;
        return;
    }

    // accept connection to client
    currentConnections++;
    socket.accept(availableInfo->getNewSocketId());
    delete msg;
}

/*
 * A peer closed the conection. Should never happen beacuse the server closes connection to clients
 */
void SurrogateServerApp::handlePeerClosed(cMessage *msg)
{

    int connId =
        check_and_cast<Indication *>(msg)->getTag<SocketInd>()->getSocketId();
    delete msg;
    auto request = new Request("close", TCP_C_CLOSE);
    request->addTag<SocketReq>()->setSocketId(connId);
    sendBack(request);
}

void SurrogateServerApp::finish()
{

    if(objectRequestTimer->isScheduled())
        cancelEvent(objectRequestTimer);
    delete objectRequestTimer;
    if(priorityTimer->isScheduled())
        cancelEvent(priorityTimer);
    delete priorityTimer;
    if(computeTimer->isScheduled())
        cancelEvent(computeTimer);
    delete computeTimer;
    if(loadTimer->isScheduled())
        cancelEvent(loadTimer);
    delete loadTimer;

    EV_INFO << getFullPath() << ": sent " << bytesSent << " bytes in "
            << msgsSent << " packets\n";
    EV_INFO << getFullPath() << ": received " << bytesRcvd << " bytes in "
            << msgsRcvd << " packets\n";
    EV_INFO << getFullPath() << ": received " << requestsFromSurrogates
            << " requests from other Surrogates\n";
    EV_INFO << endl;
}
