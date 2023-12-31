//
// Generated file, do not edit! Created by opp_msgtool 6.0 from messages/ObjectResponse.msg.
//

#ifndef __INET_OBJECTRESPONSE_M_H
#define __INET_OBJECTRESPONSE_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// opp_msgtool version check
#define MSGC_VERSION 0x0600
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgtool: 'make clean' should help.
#endif


namespace inet {

class MyObjectResponse;

}  // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/common/Units_m.h" // import inet.common.Units

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk


namespace inet {

/**
 * Class generated from <tt>messages/ObjectResponse.msg:7</tt> by opp_msgtool.
 * <pre>
 * class MyObjectResponse extends FieldsChunk
 * {
 *     int objectId;
 *     long size;
 * }
 * </pre>
 */
class MyObjectResponse : public ::inet::FieldsChunk
{
  protected:
    int objectId = 0;
    long size = 0;

  private:
    void copy(const MyObjectResponse& other);

  protected:
    bool operator==(const MyObjectResponse&) = delete;

  public:
    MyObjectResponse();
    MyObjectResponse(const MyObjectResponse& other);
    virtual ~MyObjectResponse();
    MyObjectResponse& operator=(const MyObjectResponse& other);
    virtual MyObjectResponse *dup() const override {return new MyObjectResponse(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getObjectId() const;
    virtual void setObjectId(int objectId);

    virtual long getSize() const;
    virtual void setSize(long size);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const MyObjectResponse& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, MyObjectResponse& obj) {obj.parsimUnpack(b);}


}  // namespace inet


namespace omnetpp {

template<> inline inet::MyObjectResponse *fromAnyPtr(any_ptr ptr) { return check_and_cast<inet::MyObjectResponse*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __INET_OBJECTRESPONSE_M_H

