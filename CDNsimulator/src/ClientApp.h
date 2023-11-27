//
// Copyright (C) 2004 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef __CDNSIMULATOR_CLIENTAPP_H_
#define __CDNSIMULATOR_CLIENTAPP_H_

#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/socket/SocketMap.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "definitions.h"

#include <map>
#include <vector>

using namespace inet;
using namespace std;

class ClientApp: public ApplicationBase, public TcpSocket::ICallback {
protected:

    SocketMap socketMap;
    map<int, ChunkQueue> socketQueue;
    map<int, RetryObject> retryMap; // connId, objectId

    double retryTime;
    vector<TrafficRequest> trafficRequests;
    map<int, int> clientToSurrogate; // key:=clientId, value:=surrogateId

    cMessage *trafficTimer = nullptr;

    // statistics
    int numSessions;
    int packetsSent;
    int packetsRcvd;
    int bytesSent;
    int bytesRcvd;

    // statistics:
    static simsignal_t connectSignal;
    static simsignal_t failSignal;
    static simsignal_t sendIdSignal;
    static simsignal_t receiveIdSignal;

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override {
        return NUM_INIT_STAGES;
    }
    virtual void handleRetry(int connId);
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void handleTrafficMsg();
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    /* Utility functions */
    virtual void loadRequest();
    virtual void loadClientToSurrogate();
    virtual int connect(long clientId);
    virtual void sendRequest(int objectId, int connId, long requestId);

    /* TcpSocket::ICallback callback methods */
    virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent)
            override;
    virtual void socketAvailable(TcpSocket *socket,
            TcpAvailableInfo *availableInfo) override;
    virtual void socketEstablished(TcpSocket *socket) override;
    virtual void socketPeerClosed(TcpSocket *socket) override;
    virtual void socketClosed(TcpSocket *socket) override;
    virtual void socketFailure(TcpSocket *socket, int code) override;
    virtual void socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status)
            override;
    virtual void socketDeleted(TcpSocket *socket) override {
    }

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

public:
    ClientApp() {
    }
    virtual ~ClientApp();
};

#endif
