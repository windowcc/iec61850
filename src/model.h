#ifndef __IEC_MODEL_H__
#define __IEC_MODEL_H__

#include "node.h"
// #include "internal/singleton.h"

// Da节点
class LDa : public INode
{
public:
    explicit LDa(IedConnection conn,INode *parent = nullptr);
    virtual ~LDa();

    virtual int init() final;
public:
    void setFc(const FunctionalConstraint &fc);
    FunctionalConstraint getFc() const;
    void setDataType(const MmsType &dataType);
    MmsType getDataType() const;
private:
    FunctionalConstraint _fc;
    MmsType _dataType;
};

typedef LDa LSda;

// DO节点
class LDo : public INode
{
public:
    explicit LDo(IedConnection conn,INode *parent = nullptr);
    virtual ~LDo();
    virtual int init() final;
};

using LDoPtr = std::shared_ptr<LDo>;

// LN节点
class LNode : public INode
{
public:
    explicit LNode(IedConnection conn,INode *parent = nullptr);
    virtual ~LNode();
public:
    void setDesc(const std::string &desc);
    std::string getDesc() const;

    void setLnType(const std::string &lnType);
    std::string getLnType() const;

    void setLnClass(const std::string &lnClass);
    std::string getLnClass() const;

    void setInst(const std::string &inst);
    std::string getInst() const;

    virtual int init() final;
private:
    std::string _desc;
    std::string _lnType;
    std::string _lnClass;
    std::string _inst;
};

using LNodePtr = std::shared_ptr<LNode>;

// 设备节点
class LDevice : public INode
{
public:
    explicit LDevice(IedConnection conn,INode *parent = nullptr);
    virtual ~LDevice();
public:
    void setInst(const std::string &inst);
    std::string getInst() const;

    void setDesc(const std::string &desc);
    std::string getDesc() const;

    void setLdName(const std::string &ldName);
    std::string getLdName() const;

    virtual int init() final;
private:
    std::string _inst;
    std::string _desc;
    std::string _ldName;
};

using LDevicePtr = std::shared_ptr<LDevice>;

// ---- LDevice ----
class Model : public INode
{
public:
    explicit Model(IedConnection conn);
    virtual ~Model();
    // DECL_SINGLETON(Model)
public:
    virtual int init() final;
    INode *getLdNode(const std::string &ref);
    INode *getLeafByRef(const std::string &ref,const NODE_TYPE &type,INode *leaf = nullptr);
};

// using ModelPtr = std::shared_ptr<Model>;

#endif // ! __IEC_MODEL_H__