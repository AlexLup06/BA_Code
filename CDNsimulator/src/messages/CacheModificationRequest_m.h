//
// Generated file, do not edit! Created by opp_msgtool 6.0 from messages/CacheModificationRequest.msg.
//

#ifndef __INET_CACHEMODIFICATIONREQUEST_M_H
#define __INET_CACHEMODIFICATIONREQUEST_M_H

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

class CacheModificationRequest;

}  // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/common/Units_m.h" // import inet.common.Units

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk


namespace inet {

/**
 * Class generated from <tt>messages/CacheModificationRequest.msg:7</tt> by opp_msgtool.
 * <pre>
 * class CacheModificationRequest extends FieldsChunk
 * {
 *     int type;	// 0: delete, 1: adding
 *     int objectId;	// id of object we are adding or deleting
 *     int surrogateIndex;
 *     int objectToRemove;
 * }
 * </pre>
 */
class CacheModificationRequest : public ::inet::FieldsChunk
{
  protected:
    int type = 0;
    int objectId = 0;
    int surrogateIndex = 0;
    int objectToRemove = 0;

  private:
    void copy(const CacheModificationRequest& other);

  protected:
    bool operator==(const CacheModificationRequest&) = delete;

  public:
    CacheModificationRequest();
    CacheModificationRequest(const CacheModificationRequest& other);
    virtual ~CacheModificationRequest();
    CacheModificationRequest& operator=(const CacheModificationRequest& other);
    virtual CacheModificationRequest *dup() const override {return new CacheModificationRequest(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getType() const;
    virtual void setType(int type);

    virtual int getObjectId() const;
    virtual void setObjectId(int objectId);

    virtual int getSurrogateIndex() const;
    virtual void setSurrogateIndex(int surrogateIndex);

    virtual int getObjectToRemove() const;
    virtual void setObjectToRemove(int objectToRemove);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const CacheModificationRequest& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, CacheModificationRequest& obj) {obj.parsimUnpack(b);}


}  // namespace inet


namespace omnetpp {

template<> inline inet::CacheModificationRequest *fromAnyPtr(any_ptr ptr) { return check_and_cast<inet::CacheModificationRequest*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __INET_CACHEMODIFICATIONREQUEST_M_H
