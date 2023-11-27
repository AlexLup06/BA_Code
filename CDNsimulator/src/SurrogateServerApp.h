#ifndef __CDNSIMULATOR_SURROGATESERVERAPP_H_
#define __CDNSIMULATOR_SURROGATESERVERAPP_H_

#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "definitions.h"
#include "inet/common/socket/SocketMap.h"
#include <vector>
#include <map>
#include <string>
#include "LRUCache.h"

using namespace inet;
using namespace std;


class SurrogateServerApp: public cSimpleModule, public LifecycleUnsupported {
protected:
    TcpSocket socket;
    TcpSocket *dnsSocket;

    //TODO
    SocketMap surrogateSocketRequestMap;
    SocketMap surrogateSocketResponseMap;
    TcpSocket *originSocket;
    map<int, ChunkQueue> socketQueue;

    double sendLoadInterval;
    int currentConnections = 0;
    int numSurrogates;
    long maxCacheSize;
    double computeTime;
    double priorityTime;
    short thisIndex;
    int MAX_NUMBER_CONNECTIONS;
    int requestsFromSurrogates;

    cMessage *loadTimer = nullptr;
    cMessage *computeTimer = nullptr;
    cMessage *priorityTimer = nullptr;
    cMessage *objectRequestTimer = nullptr;

    map<int,int> indexIdRequestMap; //index, socketId
    map<int,int> indexIdResponseMap; //index, socketId

    vector<ObjectRequestObject> objectRequestQueue; // queue of surrogates server request for objects
    LRUCache *cache;
    vector<Packet*> queue; // regular queue from fresh connection
    map<int, vector<int>> cacheMisses; // maps the missedObject to the connections which want that object
    vector<int> priorityQueue; // entries are missedObject; order in which connections from cacheMisses should be handled

    long msgsRcvd;
    long msgsSent;
    long bytesRcvd;
    long bytesSent;
    long fullfilled=0;
    short cachePolicy;

    static simsignal_t surrogateLoadObjects;
    static simsignal_t surrogateLoad;
    static simsignal_t surrogateObjectRequestInSignal;
    static simsignal_t surrogateObjectResponseOutSignal;
    static simsignal_t surrogateClientRequestInSignal;
    static simsignal_t surrogateClientResponseOutSignal;

protected:
    virtual void connectToSurrogates();
    virtual void connectToDNS();

    virtual void sendBack(cMessage *msg);

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override {
        return NUM_INIT_STAGES;
    }

    // handle Messages
    virtual void handleMessage(cMessage *msg) override;
    virtual void handlePeerClosed(cMessage *msg);
    virtual void handleSocketAvailable(cMessage *msg);
    virtual void handleLoadTimer();
    virtual void handleComputeTimer();
    virtual void handleClientRequest(cMessage *msg);
    void handleObjectRequest(ISocket *arrivalSocket, cMessage *msg);
    virtual void handleObjectResponse(cMessage *msg);
    virtual void handleDNSResponse(cMessage *msg);
    virtual void handleCacheMiss(int connId, int requestedObjectId);
    virtual void handlePriorityTimer();
    virtual void handleCacheModificationResponse(cMessage *msg);
    virtual void handleObjectRequestTimer();

    virtual void finish() override;
};

#endif
