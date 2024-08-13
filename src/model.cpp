#include "model.h"
#include <vector>
#include "internal/log.h"

using namespace std;

LDa::LDa(IedConnection conn,INode *parent)
    :INode(conn,NODE_DA,parent),
    _fc(IEC61850_FC_NONE)
{
}

LDa::~LDa()
{
}

int LDa::init()
{
    IedClientError err;
    FunctionalConstraint fc = _fc;
    auto lnRef = getFullRef();
    auto vspec = IedConnection_getVariableSpecification(_conn,&err,lnRef.c_str(),fc);
    if(err != IED_ERROR_OK)
    {
        LOG_ERROR("IedConnection_getVariableSpecification failed ,the error code is : " + std::to_string(err));
        if (vspec)
            MmsVariableSpecification_destroy(vspec);
        return -1;
    }
    bool ret = 0;
    auto type = MmsVariableSpecification_getType(vspec);
    if(type == MMS_STRUCTURE)
    {
        auto count = MmsVariableSpecification_getSize(vspec);
        for (int i = 0; i < count; i++)
        {
            auto item = MmsVariableSpecification_getChildSpecificationByIndex(vspec, i);
            auto name = MmsVariableSpecification_getName(item);
            if (name != NULL)
            {
                auto lSda = new LSda(_conn,this);
                lSda->setRef(name);
                lSda->setFc(fc);
                lSda->setDataType(MmsVariableSpecification_getType(item));
                if(lSda->init() == -1)
                {
                    LOG_ERROR("Init LSda failed.");
                    ret = -1;
                    break;
                }
                _childrens.push_back(lSda);
            }
            else
            {
                LOG_ERROR("MmsVariableSpecification_getName failed.");
                ret = -1;
                break;
            }
        }
    }
    else if(type == MMS_ARRAY)
    {
        int count = ::MmsVariableSpecification_getSize(vspec);
        auto item = MmsVariableSpecification_getArrayElementSpecification(vspec);
        for (int i = 0; i < count; i++)
        {
            char name[64];
            snprintf(name, sizeof(name), "(%d)", i);
            auto lSda = new LSda(_conn,this);
            lSda->setRef(name);
            lSda->setFc(fc);
            lSda->setDataType(MmsVariableSpecification_getType(item));
            if(lSda->init() == -1)
            {
                LOG_ERROR("Init LSda failed.");
            }
            _childrens.push_back(lSda);
        }
    }
    else
    {
        //LOG_WARN("LSda type is not STRUCTURE or ARRAY.");
    }
    MmsVariableSpecification_destroy(vspec);
    return ret;
}

void LDa::setFc(const FunctionalConstraint &fc)
{
    if(_fc != fc)
    {
        _fc = fc;
    }
}

FunctionalConstraint LDa::getFc() const
{
    return _fc;
}

void LDa::setDataType(const MmsType &dataType)
{
    if(_dataType != dataType)
    {
        _dataType = dataType;
    }
}

MmsType LDa::getDataType() const
{
    return _dataType;
}

LDo::LDo(IedConnection conn,INode *parent)
    :INode(conn,NODE_DO,parent)
{
}

LDo::~LDo()
{
}

int LDo::init()
{
    IedClientError err;
    auto lnRef = getFullRef();
    auto lst = IedConnection_getDataDirectoryFC(_conn,&err,lnRef.c_str());
    if(err != IED_ERROR_OK)
    {
        LOG_ERROR("IedConnection_getDataDirectoryFC failed. the error code is : " + std::to_string(err));
        if(lst)
        {
            LinkedList_destroy(lst);
        }
        return -1;
    }
    if(!lst)
    {
        LOG_ERROR("IedConnection_getDataDirectoryFC failed. the DataDiretoryFC is null.");
        return -1;
    }

    std::vector<FunctionalConstraint> fclst;
    auto it = LinkedList_getNext(lst);
    while (it)
    {
        FunctionalConstraint fc = IEC61850_FC_NONE;
        std::string name = (const char *)it->data;
        auto pos = name.rfind(']');
        if(pos != std::string::npos)
        {
            name.erase(pos);
        }
        pos = name.rfind('[');
        if (pos != std::string::npos && pos + 1 < name.size())
        {
            std::string fcStr = name.substr(pos + 1);
            fc = FunctionalConstraint_fromString(fcStr.c_str());
            if (fc != IEC61850_FC_NONE)
            {
                bool isExist = false;
                for(auto it = fclst.begin();it != fclst.end();it++)
                {
                    if(*it == fc)
                    {
                        isExist = true;
                        break;
                    }
                }
                if (!isExist)
                {
                    fclst.push_back(fc);
                }
            }
        }
        it = LinkedList_getNext(it);
    }
    LinkedList_destroy(lst);

    //根据功能约束获取所有数据的目录
    int ret = 0;
    for (auto it = fclst.begin(); it != fclst.end(); it++)
    {
        FunctionalConstraint fc = *it;
        auto vspec = IedConnection_getVariableSpecification(_conn,&err,lnRef.c_str(),fc);
        if(err != IED_ERROR_OK)
        {
            if (vspec)
                MmsVariableSpecification_destroy(vspec);
            return -1;
        }
        auto type = MmsVariableSpecification_getType(vspec);
        if(type == MMS_STRUCTURE)
        {
            auto count = MmsVariableSpecification_getSize(vspec);
            for (int i = 0; i < count; i++)
            {
                auto item = MmsVariableSpecification_getChildSpecificationByIndex(vspec, i);
                auto name = MmsVariableSpecification_getName(item);
                if (name != NULL)
                {
                    auto lDa = new LDa(_conn,this);
                    lDa->setRef(name);
                    lDa->setFc(fc);
                    lDa->setDataType(MmsVariableSpecification_getType(item));
                    if(lDa->init() == -1)
                    {
                        LOG_ERROR("Init LDa failed");
                        ret = -1;
                    }
                    _childrens.push_back(lDa);
                }
            }
        }
        else
        {
            LOG_WARN("LDa type is not STRUCTURE or ARRAY.");
        }
        MmsVariableSpecification_destroy(vspec);
    }
    return ret;
}

LNode::LNode(IedConnection conn,INode *parent)
    :INode(conn,NODE_LN,parent)
{

}

LNode::~LNode()
{

}

void LNode::setDesc(const std::string &desc)
{
    if(_desc != desc)
    {
        _desc = desc;
    }
}

std::string LNode::getDesc() const
{
    return _desc;
}

void LNode::setLnType(const std::string &lnType)
{
    if(_lnType != lnType)
    {
        _lnType = lnType;
    }
}

std::string LNode::getLnType() const
{
    return _lnType;
}

void LNode::setLnClass(const std::string &lnClass)
{
    if(_lnClass != lnClass)
    {
        _lnClass = _lnClass;
    }
}

std::string LNode::getLnClass() const
{
    return _lnClass;
}

void LNode::setInst(const std::string &inst)
{
    if(_inst != inst)
    {
        _inst = inst;
    }
}

std::string LNode::getInst() const
{
    return _inst;
}

int LNode::init()
{
    IedClientError err;
    auto lnRef = getFullRef();
    auto lst = IedConnection_getLogicalNodeDirectory(_conn, &err,lnRef.c_str(), ACSI_CLASS_DATA_OBJECT);
    if(err != IED_ERROR_OK)
    {
        LOG_ERROR("IedConnection_getLogicalNodeDirectory failed. the error code is : " + std::to_string(err));
        if(lst)
        {
            LinkedList_destroy(lst);
        }
        return -1;
    }
    if(!lst)
    {
        LOG_ERROR("IedConnection_getLogicalNodeDirectory failed. the LogicNodeDirectory is null.");
        return -1;
    }

    int ret = 0;
    auto it = LinkedList_getNext(lst);
    while (it)
    {
        auto lDo = new LDo(_conn,this);
        std::string name = (const char *)it->data;
        lDo->setRef((const char *)it->data);
        if(lDo->init() != 0)
        {
            LOG_ERROR("lDo init failed.");
            ret = -1;
            break;
        }
        _childrens.push_back(lDo);
        it = LinkedList_getNext(it);
    }
    LinkedList_destroy(lst);
    return ret;
}

LDevice::LDevice(IedConnection conn,INode *parent)
    :INode(conn,NODE_LD,parent)
{

}

LDevice::~LDevice()
{

}

void LDevice::setInst(const std::string &inst)
{
    if(_inst != inst)
    {
        _inst = inst;
    }
}

std::string LDevice::getInst() const
{
    return _inst;
}

void LDevice::setDesc(const std::string &desc)
{
    if(_desc != desc)
    {
        _desc = desc;
    }
}

std::string LDevice::getDesc() const
{
    return _desc;
}

void LDevice::setLdName(const std::string &ldName)
{
    if(_ldName != ldName)
    {
        _ldName = ldName;
    }
}

std::string LDevice::getLdName() const
{
    return _ldName;
}

int LDevice::init()
{
    IedClientError err;
    auto lst = IedConnection_getLogicalDeviceDirectory(_conn,&err,_ref.c_str());
    if(err != IED_ERROR_OK)
    {
        LOG_ERROR("IedConnection_getLogicalDeviceDirectory failed, the error code is :" + std::to_string(err));
        if(lst)
        {
            LinkedList_destroy(lst);
        }
        return -1;
    }
    if(!lst)
    {
        LOG_ERROR("IedConnection_getLogicalDeviceDirectory failed, LogicalDeviceDirectory is null");
        return -1;
    }
    int ret = 0;
    auto it = LinkedList_getNext(lst);
    while (it)
    {
        auto node = new LNode(_conn,this);
        std::string name = (const char *)it->data;
        node->setRef((const char *)it->data);
        if(node->init() != 0)
        {
            LOG_ERROR("Init LNode failed.");
            ret = -1;
            break;
        }
        _childrens.push_back(node);
        it = LinkedList_getNext(it);
    }
    LinkedList_destroy(lst);
    return ret;
}

Model::Model(IedConnection conn)
    :INode(conn,NODE_INVALID,nullptr)
{
}

Model::~Model()
{

}

int Model::init()
{
    IedClientError err;
    auto lst = IedConnection_getLogicalDeviceList(_conn,&err);
    if(err != IED_ERROR_OK)
    {
        LOG_ERROR("IedConnection_getLogicalDeviceList faild.the error code is : " + std::to_string(err));
        if(lst)
            LinkedList_destroy(lst);
        return -1;
    }
    if(!lst)
    {
        LOG_ERROR("IedConnection_getLogicalDeviceList faild.the LogicalDeviceList is null.");
        return -1;
    }
    int ret = 0;
    auto it = LinkedList_getNext(lst);
    while (it)
    {
        auto ld = new LDevice(_conn,this);
        ld->setRef((const char *)it->data);
        if(ld->init() != 0)
        {
            LOG_ERROR("Init LN failed.");
            ret = -1;
            break;
        }
        _childrens.push_back(ld);
        it = LinkedList_getNext(it);
    }
    LinkedList_destroy(lst);
    return 0;
}

INode *Model::getLdNode(const std::string &ref)
{
    return nullptr;
}

INode *Model::getLeafByRef(const std::string &ref,const NODE_TYPE &type,INode *leaf)
{
    INode *node = nullptr;
    if(!leaf)
    {
        leaf = this;
    }
    auto childrens = leaf->_childrens;
    for(auto it = childrens.begin() ; it != childrens.end(); it++)
    {
        auto leafType = (*it)->getType();
        if(leafType != type)
        {
            node = getLeafByRef(ref,type,*it);
            if(node)
            {
                break;
            }
        }
        else
        {
            auto fullRef = (*it)->getFullRef();
            if(fullRef == ref)
            {
                node = *it;
                break;
            }
        }
    }
    return node;
}
