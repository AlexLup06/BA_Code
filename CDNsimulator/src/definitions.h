#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <omnetpp.h>

struct CacheObject {
    long id;
    long size;

    CacheObject(long _id, long _size) :
            id(_id), size(_size) {
    }
};

struct QueueObject {
    int connId;
    int objectId;

    QueueObject(int _connId, int _objectId) :
            connId(_connId), objectId(_objectId) {
    }
};

struct TrafficRequest {
    long clientId;
    long objectId;
    long requestId;
    long timeStamp;

    TrafficRequest(long _requestId, long _clientId, long _objectId,
            long _timeStamp) :
            requestId(_requestId), clientId(_clientId), objectId(_objectId), timeStamp(
                    _timeStamp) {
    }
};

struct ObjectRequestObject {
    int objectSize;
    int requestedObjectId;
    int connId;

    ObjectRequestObject(int _objectSize, int _requestedObjectId, int _connId) :
            objectSize(_objectSize), requestedObjectId(_requestedObjectId), connId(
                    _connId) {
    }
};

struct RetryObject {
    long objectId;
    long clientId;
    long requestId;

    RetryObject(long _objectId, long _clientId, long _requestId) :
            objectId(_objectId), clientId(_clientId), requestId(_requestId) {
    }
};

#include <fstream>
#include <string>

inline void writeToLog(omnetpp::cModule *mod, const std::string &message) {
    std::ofstream outputFile(
            mod->getSystemModule()->par("logFile").stringValue(),
            std::ios::app);

    if (!outputFile) {
        throw omnetpp::cRuntimeError("Could not open the log file");
    }

    outputFile << mod->getFullPath() <<" at " << omnetpp::simTime() << " : " << message << std::endl;
    outputFile.close();
}

#endif
