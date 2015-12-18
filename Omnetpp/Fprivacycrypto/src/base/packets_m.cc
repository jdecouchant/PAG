//
// Generated file, do not edit! Created by opp_msgc 4.2 from base/packets.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "packets_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(HistoryPacket);

HistoryPacket::HistoryPacket(const char *name, int kind) : cPacket(name,kind)
{
    this->msgType_var = 0;
    this->senderId_var = 0;
    this->receiverId_var = 0;
    updatesId_arraysize = 0;
    this->updatesId_var = 0;
}

HistoryPacket::HistoryPacket(const HistoryPacket& other) : cPacket(other)
{
    updatesId_arraysize = 0;
    this->updatesId_var = 0;
    copy(other);
}

HistoryPacket::~HistoryPacket()
{
    delete [] updatesId_var;
}

HistoryPacket& HistoryPacket::operator=(const HistoryPacket& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void HistoryPacket::copy(const HistoryPacket& other)
{
    this->msgType_var = other.msgType_var;
    this->senderId_var = other.senderId_var;
    this->receiverId_var = other.receiverId_var;
    delete [] this->updatesId_var;
    this->updatesId_var = (other.updatesId_arraysize==0) ? NULL : new int[other.updatesId_arraysize];
    updatesId_arraysize = other.updatesId_arraysize;
    for (unsigned int i=0; i<updatesId_arraysize; i++)
        this->updatesId_var[i] = other.updatesId_var[i];
}

void HistoryPacket::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->msgType_var);
    doPacking(b,this->senderId_var);
    doPacking(b,this->receiverId_var);
    b->pack(updatesId_arraysize);
    doPacking(b,this->updatesId_var,updatesId_arraysize);
}

void HistoryPacket::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->msgType_var);
    doUnpacking(b,this->senderId_var);
    doUnpacking(b,this->receiverId_var);
    delete [] this->updatesId_var;
    b->unpack(updatesId_arraysize);
    if (updatesId_arraysize==0) {
        this->updatesId_var = 0;
    } else {
        this->updatesId_var = new int[updatesId_arraysize];
        doUnpacking(b,this->updatesId_var,updatesId_arraysize);
    }
}

int8_t HistoryPacket::getMsgType() const
{
    return msgType_var;
}

void HistoryPacket::setMsgType(int8_t msgType)
{
    this->msgType_var = msgType;
}

int HistoryPacket::getSenderId() const
{
    return senderId_var;
}

void HistoryPacket::setSenderId(int senderId)
{
    this->senderId_var = senderId;
}

int HistoryPacket::getReceiverId() const
{
    return receiverId_var;
}

void HistoryPacket::setReceiverId(int receiverId)
{
    this->receiverId_var = receiverId;
}

void HistoryPacket::setUpdatesIdArraySize(unsigned int size)
{
    int *updatesId_var2 = (size==0) ? NULL : new int[size];
    unsigned int sz = updatesId_arraysize < size ? updatesId_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        updatesId_var2[i] = this->updatesId_var[i];
    for (unsigned int i=sz; i<size; i++)
        updatesId_var2[i] = 0;
    updatesId_arraysize = size;
    delete [] this->updatesId_var;
    this->updatesId_var = updatesId_var2;
}

unsigned int HistoryPacket::getUpdatesIdArraySize() const
{
    return updatesId_arraysize;
}

int HistoryPacket::getUpdatesId(unsigned int k) const
{
    if (k>=updatesId_arraysize) throw cRuntimeError("Array of size %d indexed by %d", updatesId_arraysize, k);
    return updatesId_var[k];
}

void HistoryPacket::setUpdatesId(unsigned int k, int updatesId)
{
    if (k>=updatesId_arraysize) throw cRuntimeError("Array of size %d indexed by %d", updatesId_arraysize, k);
    this->updatesId_var[k] = updatesId;
}

class HistoryPacketDescriptor : public cClassDescriptor
{
  public:
    HistoryPacketDescriptor();
    virtual ~HistoryPacketDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(HistoryPacketDescriptor);

HistoryPacketDescriptor::HistoryPacketDescriptor() : cClassDescriptor("HistoryPacket", "cPacket")
{
}

HistoryPacketDescriptor::~HistoryPacketDescriptor()
{
}

bool HistoryPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<HistoryPacket *>(obj)!=NULL;
}

const char *HistoryPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int HistoryPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int HistoryPacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *HistoryPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "msgType",
        "senderId",
        "receiverId",
        "updatesId",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int HistoryPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "msgType")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderId")==0) return base+1;
    if (fieldName[0]=='r' && strcmp(fieldName, "receiverId")==0) return base+2;
    if (fieldName[0]=='u' && strcmp(fieldName, "updatesId")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *HistoryPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int8_t",
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *HistoryPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int HistoryPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    HistoryPacket *pp = (HistoryPacket *)object; (void)pp;
    switch (field) {
        case 3: return pp->getUpdatesIdArraySize();
        default: return 0;
    }
}

std::string HistoryPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    HistoryPacket *pp = (HistoryPacket *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getMsgType());
        case 1: return long2string(pp->getSenderId());
        case 2: return long2string(pp->getReceiverId());
        case 3: return long2string(pp->getUpdatesId(i));
        default: return "";
    }
}

bool HistoryPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    HistoryPacket *pp = (HistoryPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setMsgType(string2long(value)); return true;
        case 1: pp->setSenderId(string2long(value)); return true;
        case 2: pp->setReceiverId(string2long(value)); return true;
        case 3: pp->setUpdatesId(i,string2long(value)); return true;
        default: return false;
    }
}

const char *HistoryPacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<4) ? fieldStructNames[field] : NULL;
}

void *HistoryPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    HistoryPacket *pp = (HistoryPacket *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(BriefPacket);

BriefPacket::BriefPacket(const char *name, int kind) : cPacket(name,kind)
{
    this->msgType_var = 0;
    this->senderId_var = 0;
    updatesId_arraysize = 0;
    this->updatesId_var = 0;
}

BriefPacket::BriefPacket(const BriefPacket& other) : cPacket(other)
{
    updatesId_arraysize = 0;
    this->updatesId_var = 0;
    copy(other);
}

BriefPacket::~BriefPacket()
{
    delete [] updatesId_var;
}

BriefPacket& BriefPacket::operator=(const BriefPacket& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void BriefPacket::copy(const BriefPacket& other)
{
    this->msgType_var = other.msgType_var;
    this->senderId_var = other.senderId_var;
    delete [] this->updatesId_var;
    this->updatesId_var = (other.updatesId_arraysize==0) ? NULL : new int[other.updatesId_arraysize];
    updatesId_arraysize = other.updatesId_arraysize;
    for (unsigned int i=0; i<updatesId_arraysize; i++)
        this->updatesId_var[i] = other.updatesId_var[i];
}

void BriefPacket::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->msgType_var);
    doPacking(b,this->senderId_var);
    b->pack(updatesId_arraysize);
    doPacking(b,this->updatesId_var,updatesId_arraysize);
}

void BriefPacket::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->msgType_var);
    doUnpacking(b,this->senderId_var);
    delete [] this->updatesId_var;
    b->unpack(updatesId_arraysize);
    if (updatesId_arraysize==0) {
        this->updatesId_var = 0;
    } else {
        this->updatesId_var = new int[updatesId_arraysize];
        doUnpacking(b,this->updatesId_var,updatesId_arraysize);
    }
}

int8_t BriefPacket::getMsgType() const
{
    return msgType_var;
}

void BriefPacket::setMsgType(int8_t msgType)
{
    this->msgType_var = msgType;
}

int BriefPacket::getSenderId() const
{
    return senderId_var;
}

void BriefPacket::setSenderId(int senderId)
{
    this->senderId_var = senderId;
}

void BriefPacket::setUpdatesIdArraySize(unsigned int size)
{
    int *updatesId_var2 = (size==0) ? NULL : new int[size];
    unsigned int sz = updatesId_arraysize < size ? updatesId_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        updatesId_var2[i] = this->updatesId_var[i];
    for (unsigned int i=sz; i<size; i++)
        updatesId_var2[i] = 0;
    updatesId_arraysize = size;
    delete [] this->updatesId_var;
    this->updatesId_var = updatesId_var2;
}

unsigned int BriefPacket::getUpdatesIdArraySize() const
{
    return updatesId_arraysize;
}

int BriefPacket::getUpdatesId(unsigned int k) const
{
    if (k>=updatesId_arraysize) throw cRuntimeError("Array of size %d indexed by %d", updatesId_arraysize, k);
    return updatesId_var[k];
}

void BriefPacket::setUpdatesId(unsigned int k, int updatesId)
{
    if (k>=updatesId_arraysize) throw cRuntimeError("Array of size %d indexed by %d", updatesId_arraysize, k);
    this->updatesId_var[k] = updatesId;
}

class BriefPacketDescriptor : public cClassDescriptor
{
  public:
    BriefPacketDescriptor();
    virtual ~BriefPacketDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(BriefPacketDescriptor);

BriefPacketDescriptor::BriefPacketDescriptor() : cClassDescriptor("BriefPacket", "cPacket")
{
}

BriefPacketDescriptor::~BriefPacketDescriptor()
{
}

bool BriefPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<BriefPacket *>(obj)!=NULL;
}

const char *BriefPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int BriefPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int BriefPacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *BriefPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "msgType",
        "senderId",
        "updatesId",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int BriefPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "msgType")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderId")==0) return base+1;
    if (fieldName[0]=='u' && strcmp(fieldName, "updatesId")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *BriefPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int8_t",
        "int",
        "int",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *BriefPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int BriefPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    BriefPacket *pp = (BriefPacket *)object; (void)pp;
    switch (field) {
        case 2: return pp->getUpdatesIdArraySize();
        default: return 0;
    }
}

std::string BriefPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    BriefPacket *pp = (BriefPacket *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getMsgType());
        case 1: return long2string(pp->getSenderId());
        case 2: return long2string(pp->getUpdatesId(i));
        default: return "";
    }
}

bool BriefPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    BriefPacket *pp = (BriefPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setMsgType(string2long(value)); return true;
        case 1: pp->setSenderId(string2long(value)); return true;
        case 2: pp->setUpdatesId(i,string2long(value)); return true;
        default: return false;
    }
}

const char *BriefPacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *BriefPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    BriefPacket *pp = (BriefPacket *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


