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

#ifndef __CDNSIMULATOR_DNSAPP_H_
#define __CDNSIMULATOR_DNSAPP_H_

#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "definitions.h"
#include "inet/common/socket/SocketMap.h"

#include <vector>
#include <unordered_set>

using namespace inet;
using namespace std;

class DNSApp: public cSimpleModule, public LifecycleUnsupported {
protected:
    TcpSocket socket;
    SocketMap surrogateSocketMap;   // connections to surrogate Servers
    TcpSocket originSocket;

    vector<int> serverLoads;   // each index is Id of surrogate server
    vector<unordered_set<int>> serverCaches; // each index is Id of surrogate server

    cModule *networkModulePtr;
    int numSurrogates;
    short cachePolicy;

    long msgsRcvd;
    long msgsSent;
    long bytesRcvd;
    long bytesSent;
    double computeTime;

    vector<vector<short>> otherSurrogatesSortedByDistance;
    vector<Packet*> queue; // queue all the  packets arriving at DNS

protected:

    virtual void sendBack(cMessage *msg);

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override {
        return NUM_INIT_STAGES;
    }

    virtual void loadOtherSurrogates();

    // handle Messages
    virtual void handleMessage(cMessage *msg) override;
    virtual void handlePeerClosed(cMessage *msg);
    virtual void handleSocketAvailable(cMessage *msg);
    virtual void handleLoadMessage(ISocket *arrivalSocket, cMessage *msg);
    virtual void handleSurrogateRequest(ISocket *arrivalSocket, cMessage *msg);
    virtual void handleCacheModification(ISocket *arrivalSocket, cMessage *msg);

    virtual void finish() override;
};

#endif
