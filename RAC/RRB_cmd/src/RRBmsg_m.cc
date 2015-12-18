//
// Generated file, do not edit! Created by opp_msgc 4.2 from RRBmsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "RRBmsg_m.h"

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




Register_Class(RRBmsg);

RRBmsg::RRBmsg(const char *name, int kind) : cPacket(name,kind)
{
    this->RRBmsg_id_var = 0;
}

RRBmsg::RRBmsg(const RRBmsg& other) : cPacket(other)
{
    copy(other);
}

RRBmsg::~RRBmsg()
{
}

RRBmsg& RRBmsg::operator=(const RRBmsg& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void RRBmsg::copy(const RRBmsg& other)
{
    this->origine_var = other.origine_var;
    this->src_var = other.src_var;
    this->RRBmsg_id_var = other.RRBmsg_id_var;
}

void RRBmsg::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->origine_var);
    doPacking(b,this->src_var);
    doPacking(b,this->RRBmsg_id_var);
}

void RRBmsg::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->origine_var);
    doUnpacking(b,this->src_var);
    doUnpacking(b,this->RRBmsg_id_var);
}

Node& RRBmsg::getOrigine()
{
    return origine_var;
}

void RRBmsg::setOrigine(const Node& origine)
{
    this->origine_var = origine;
}

Node& RRBmsg::getSrc()
{
    return src_var;
}

void RRBmsg::setSrc(const Node& src)
{
    this->src_var = src;
}

int RRBmsg::getRRBmsg_id() const
{
    return RRBmsg_id_var;
}

void RRBmsg::setRRBmsg_id(int RRBmsg_id)
{
    this->RRBmsg_id_var = RRBmsg_id;
}

class RRBmsgDescriptor : public cClassDescriptor
{
  public:
    RRBmsgDescriptor();
    virtual ~RRBmsgDescriptor();

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

Register_ClassDescriptor(RRBmsgDescriptor);

RRBmsgDescriptor::RRBmsgDescriptor() : cClassDescriptor("RRBmsg", "cPacket")
{
}

RRBmsgDescriptor::~RRBmsgDescriptor()
{
}

bool RRBmsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<RRBmsg *>(obj)!=NULL;
}

const char *RRBmsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int RRBmsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int RRBmsgDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *RRBmsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "origine",
        "src",
        "RRBmsg_id",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int RRBmsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='o' && strcmp(fieldName, "origine")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "src")==0) return base+1;
    if (fieldName[0]=='R' && strcmp(fieldName, "RRBmsg_id")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *RRBmsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "Node",
        "Node",
        "int",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *RRBmsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int RRBmsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    RRBmsg *pp = (RRBmsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string RRBmsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    RRBmsg *pp = (RRBmsg *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getOrigine(); return out.str();}
        case 1: {std::stringstream out; out << pp->getSrc(); return out.str();}
        case 2: return long2string(pp->getRRBmsg_id());
        default: return "";
    }
}

bool RRBmsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    RRBmsg *pp = (RRBmsg *)object; (void)pp;
    switch (field) {
        case 2: pp->setRRBmsg_id(string2long(value)); return true;
        default: return false;
    }
}

const char *RRBmsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        "Node",
        "Node",
        NULL,
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *RRBmsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    RRBmsg *pp = (RRBmsg *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getOrigine()); break;
        case 1: return (void *)(&pp->getSrc()); break;
        default: return NULL;
    }
}


