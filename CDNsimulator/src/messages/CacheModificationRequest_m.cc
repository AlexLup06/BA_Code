//
// Generated file, do not edit! Created by opp_msgtool 6.0 from messages/CacheModificationRequest.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "CacheModificationRequest_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace inet {

Register_Class(CacheModificationRequest)

CacheModificationRequest::CacheModificationRequest() : ::inet::FieldsChunk()
{
}

CacheModificationRequest::CacheModificationRequest(const CacheModificationRequest& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

CacheModificationRequest::~CacheModificationRequest()
{
}

CacheModificationRequest& CacheModificationRequest::operator=(const CacheModificationRequest& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void CacheModificationRequest::copy(const CacheModificationRequest& other)
{
    this->type = other.type;
    this->objectId = other.objectId;
    this->surrogateIndex = other.surrogateIndex;
    this->objectToRemove = other.objectToRemove;
}

void CacheModificationRequest::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->type);
    doParsimPacking(b,this->objectId);
    doParsimPacking(b,this->surrogateIndex);
    doParsimPacking(b,this->objectToRemove);
}

void CacheModificationRequest::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->type);
    doParsimUnpacking(b,this->objectId);
    doParsimUnpacking(b,this->surrogateIndex);
    doParsimUnpacking(b,this->objectToRemove);
}

int CacheModificationRequest::getType() const
{
    return this->type;
}

void CacheModificationRequest::setType(int type)
{
    handleChange();
    this->type = type;
}

int CacheModificationRequest::getObjectId() const
{
    return this->objectId;
}

void CacheModificationRequest::setObjectId(int objectId)
{
    handleChange();
    this->objectId = objectId;
}

int CacheModificationRequest::getSurrogateIndex() const
{
    return this->surrogateIndex;
}

void CacheModificationRequest::setSurrogateIndex(int surrogateIndex)
{
    handleChange();
    this->surrogateIndex = surrogateIndex;
}

int CacheModificationRequest::getObjectToRemove() const
{
    return this->objectToRemove;
}

void CacheModificationRequest::setObjectToRemove(int objectToRemove)
{
    handleChange();
    this->objectToRemove = objectToRemove;
}

class CacheModificationRequestDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_type,
        FIELD_objectId,
        FIELD_surrogateIndex,
        FIELD_objectToRemove,
    };
  public:
    CacheModificationRequestDescriptor();
    virtual ~CacheModificationRequestDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(CacheModificationRequestDescriptor)

CacheModificationRequestDescriptor::CacheModificationRequestDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::CacheModificationRequest)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

CacheModificationRequestDescriptor::~CacheModificationRequestDescriptor()
{
    delete[] propertyNames;
}

bool CacheModificationRequestDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CacheModificationRequest *>(obj)!=nullptr;
}

const char **CacheModificationRequestDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CacheModificationRequestDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CacheModificationRequestDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int CacheModificationRequestDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_type
        FD_ISEDITABLE,    // FIELD_objectId
        FD_ISEDITABLE,    // FIELD_surrogateIndex
        FD_ISEDITABLE,    // FIELD_objectToRemove
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *CacheModificationRequestDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "type",
        "objectId",
        "surrogateIndex",
        "objectToRemove",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int CacheModificationRequestDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "type") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "objectId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "surrogateIndex") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "objectToRemove") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *CacheModificationRequestDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_type
        "int",    // FIELD_objectId
        "int",    // FIELD_surrogateIndex
        "int",    // FIELD_objectToRemove
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **CacheModificationRequestDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *CacheModificationRequestDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int CacheModificationRequestDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CacheModificationRequest *pp = omnetpp::fromAnyPtr<CacheModificationRequest>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CacheModificationRequestDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CacheModificationRequest *pp = omnetpp::fromAnyPtr<CacheModificationRequest>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CacheModificationRequest'", field);
    }
}

const char *CacheModificationRequestDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CacheModificationRequest *pp = omnetpp::fromAnyPtr<CacheModificationRequest>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CacheModificationRequestDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CacheModificationRequest *pp = omnetpp::fromAnyPtr<CacheModificationRequest>(object); (void)pp;
    switch (field) {
        case FIELD_type: return long2string(pp->getType());
        case FIELD_objectId: return long2string(pp->getObjectId());
        case FIELD_surrogateIndex: return long2string(pp->getSurrogateIndex());
        case FIELD_objectToRemove: return long2string(pp->getObjectToRemove());
        default: return "";
    }
}

void CacheModificationRequestDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CacheModificationRequest *pp = omnetpp::fromAnyPtr<CacheModificationRequest>(object); (void)pp;
    switch (field) {
        case FIELD_type: pp->setType(string2long(value)); break;
        case FIELD_objectId: pp->setObjectId(string2long(value)); break;
        case FIELD_surrogateIndex: pp->setSurrogateIndex(string2long(value)); break;
        case FIELD_objectToRemove: pp->setObjectToRemove(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CacheModificationRequest'", field);
    }
}

omnetpp::cValue CacheModificationRequestDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CacheModificationRequest *pp = omnetpp::fromAnyPtr<CacheModificationRequest>(object); (void)pp;
    switch (field) {
        case FIELD_type: return pp->getType();
        case FIELD_objectId: return pp->getObjectId();
        case FIELD_surrogateIndex: return pp->getSurrogateIndex();
        case FIELD_objectToRemove: return pp->getObjectToRemove();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CacheModificationRequest' as cValue -- field index out of range?", field);
    }
}

void CacheModificationRequestDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CacheModificationRequest *pp = omnetpp::fromAnyPtr<CacheModificationRequest>(object); (void)pp;
    switch (field) {
        case FIELD_type: pp->setType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_objectId: pp->setObjectId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_surrogateIndex: pp->setSurrogateIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_objectToRemove: pp->setObjectToRemove(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CacheModificationRequest'", field);
    }
}

const char *CacheModificationRequestDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr CacheModificationRequestDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CacheModificationRequest *pp = omnetpp::fromAnyPtr<CacheModificationRequest>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CacheModificationRequestDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CacheModificationRequest *pp = omnetpp::fromAnyPtr<CacheModificationRequest>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CacheModificationRequest'", field);
    }
}

}  // namespace inet

namespace omnetpp {

}  // namespace omnetpp

