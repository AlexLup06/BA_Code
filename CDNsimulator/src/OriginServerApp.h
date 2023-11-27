//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __CDNSIMULATOR_ORIGINSERVERAPP_H_
#define __CDNSIMULATOR_ORIGINSERVERAPP_H_

#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/common/socket/SocketMap.h"

#include <vector>
#include <map>

#include "definitions.h"

using namespace inet;
using namespace std;

/**
 * Generic server application. It serves requests coming in GenericAppMsg
 * request messages. Clients are usually subclassed from TcpAppBase.
 *
 * @see GenericAppMsg, TcpAppBase
 */
class OriginServerApp: public cSimpleModule, public LifecycleUnsupported {
protected:
    TcpSocket socket;
    SocketMap surrogateSocketMap;

    static simsignal_t loadInfoOriginSignal;
    static simsignal_t originRequestInSignal;
    static simsignal_t originResponseOutSignal;

    long msgsRcvd;
    long msgsSent;
    long bytesRcvd;
    long bytesSent;
    double computeTime;
    int numSurrogates = 0;
    double sendLoadInterval;

    cMessage *loadTimer = nullptr;
    cMessage *computeTimer = nullptr;


    vector<QueueObject> queue;
    vector<CacheObject> objects;
    map<int, ChunkQueue> socketQueue;

protected:
    virtual void loadObjects();
    virtual void createTcpConnectionToSurrogates();

    virtual void sendBack(cMessage *msg);
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override {
        return NUM_INIT_STAGES;
    }
    virtual void handleComputeTimer();
    virtual void handleMessage(cMessage *msg) override;
    virtual void handleSurrogateRequest(ISocket *arrivalSocket, cMessage *msg);
    virtual void handlePeerClosed(cMessage *msg);
    virtual void handleLoadTimer();

    virtual void finish() override;
};

#endif

