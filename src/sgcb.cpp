#include "sgcb.h"
#include "model.h"
#include "internal/log.h"


static std::string MmsValue2String(const MmsValue *value,const uint32_t &maxsize = 1024)
{
    if(!value)
    {
        return "";
    }
    std::string str(maxsize, '\0');
    ::MmsValue_printToBuffer(value, (char*)str.data(), str.size());
    str.resize(strlen(str.c_str()));
    return str;
}

ISgcbRW::ISgcbRW()
{

}

ISgcbRW::~ISgcbRW()
{

}

int ISgcbRW::writeData(IedConnection conn,const std::string &sg_ref, const FunctionalConstraint &fc, MmsValue *value)
{
    IedClientError err = IED_ERROR_OK;
    if(!conn || !value)
    {
        LOG_ERROR("Iec client connection is null or value is null.");
        return -1;
    }

    IedConnection_writeObject(conn,&err,sg_ref.c_str(),fc,value);
    if(err != IED_ERROR_OK)
    {
        LOG_ERROR("IedConnection_writeObject failed, the error code is : [ " + std::to_string(err) + " ]");
        return -1;
    }
    return 0;
}

MmsValue *ISgcbRW::readData(IedConnection conn,const std::string &sg_ref,const FunctionalConstraint &fc)
{
    IedClientError err = IED_ERROR_OK;
    if(!conn)
    {
        LOG_ERROR("Iec client connection is null.");
        return nullptr;
    }
    MmsValue *val = IedConnection_readObject(conn,&err,sg_ref.c_str(),fc);
    if(err != IED_ERROR_OK)
    {
        LOG_ERROR("IedConnection_readObject faild,the error code is : [ " + std::to_string(err) + " ]");
        return nullptr;
    }
    return val;
}

Sgcb::Sgcb(IedConnection conn,INode *parent)
    :INode(conn,NODE_SGCB,parent)
{

}

Sgcb::~Sgcb()
{
    if(_parent)
    {
        _parent = nullptr;
    }
    //!!!! 只讲指针指向nullptr,  防止Model释放内存导致double free错误.
    if(!_childrens.empty())
    {
        for (auto it = _childrens.begin(); it != _childrens.end(); it++)
        {
            *it = nullptr;
        }
        _childrens.clear();
    }
}

int Sgcb::init()
{
    return update();
}

int Sgcb::update()
{
    IedClientError err;
    for (auto it = _childrens.begin(); it != _childrens.end(); it++)
    {
        auto object = *it;
        auto fullRef = object->getFullRef();
        auto mmsValue = readData(_conn,fullRef.c_str(),IEC61850_FC_SP);
        if(!mmsValue)
        {
            LOG_ERROR("Read SGCB property failed,the value is null");
            return -1;
        }
        auto ref = object->getRef();
        if (ref == "NumOfSG")
        {
            setNumOfSg((uint8_t)MmsValue_toUint32(mmsValue));
        }
        else if (ref == "ActSG")
        {
            setActSg((uint8_t)MmsValue_toUint32(mmsValue));
        }
        else if (ref == "EditSG")
        {
            setEditSg((uint8_t)MmsValue_toUint32(mmsValue));
        }
        else if (ref == "CnfEdit")
        {
            setConfirm(MmsValue_getBoolean(mmsValue));
        }
        else if (ref == "LActTm")
        {
            uint32_t us = 0;
            uint64_t ms = MmsValue_getUtcTimeInMsWithUs(mmsValue, &us);
            setActTm((time_t)(ms / 1000));
            setActTmUs((suseconds_t)((ms % 1000) * 1000 + us));
        }
        else if (ref == "ResvTms")
        {
            setResvTms((uint16_t)MmsValue_toUint32(mmsValue));
        }
        else
        {
            LOG_ERROR("Invalid SGCB property. the property is : [ " + MmsValue2String(mmsValue) + " ]");
        }
        MmsValue_delete(mmsValue);
    }
    return 0;
}

int Sgcb::writeNumSg(const uint8_t &numSg)
{
    auto it = _childrens.begin();
    for (; it != _childrens.end(); it++)
    {
        auto node = *it;
        if(node->getRef() == "NumOfSG")
        {
            break;
        }
    }
    if(it == _childrens.end())
    {
        LOG_ERROR("Write NumSg failed,Not found NumOfSg");
        return -1;
    }

    MmsValue *value = MmsValue_newUnsignedFromUint32(numSg);
    auto ret = writeData(_conn,(*it)->getFullRef(),IEC61850_FC_SP,value);
    if(ret != 0)
    {
        LOG_ERROR("Write NumSg failed.");
        return -1;
    }
    setNumOfSg(numSg);
    MmsValue_delete(value);
    return 0;
}

int Sgcb::writeActSg(const uint8_t &actSg)
{
    if(actSg <=0 || actSg > _numOfSg)
    {
        LOG_ERROR("Write ActSg failed,actSg out of range.");
        return -1;
    }
    auto it = _childrens.begin();
    for (; it != _childrens.end(); it++)
    {
        auto node = *it;
        if(node->getRef() == "ActSG")
        {
            break;
        }
    }
    if(it == _childrens.end())
    {
        LOG_ERROR("Write ActSg failed,Not found ActSg");
        return -1;
    }

    MmsValue *value = MmsValue_newUnsignedFromUint32(actSg);
    auto ret = writeData(_conn,(*it)->getFullRef(),IEC61850_FC_SP,value);
    if(ret != 0)
    {
        LOG_ERROR("Write ActSg failed.");
        return -1;
    }
    setActSg(actSg);
    MmsValue_delete(value);
    return 0;
}

int Sgcb::writeEditSg(const uint8_t &editSg)
{
    if(editSg > _numOfSg || editSg < 0)
    {
        LOG_ERROR("Write editSg failed,editSg out of range.");
        return -1;
    }
    auto it = _childrens.begin();
    for (; it != _childrens.end(); it++)
    {
        auto node = *it;
        if(node->getRef() == "EditSG")
        {
            break;
        }
    }
    if(it == _childrens.end())
    {
        LOG_ERROR("Write editSg failed,editSg not found.");
        return -1;
    }

    MmsValue *value = MmsValue_newUnsignedFromUint32(editSg);
    auto ret = writeData(_conn,(*it)->getFullRef(),IEC61850_FC_SP,value);
    if(ret != 0)
    {
        LOG_ERROR("Write editSg failed.");
        return -1;
    }
    setEditSg(editSg);
    MmsValue_delete(value);
    return 0;
}

int Sgcb::writeConfirm(const bool &confirm)
{
    auto it = _childrens.begin();
    for (; it != _childrens.end(); it++)
    {
        auto node = *it;
        if(node->getRef() == "CnfEdit")
        {
            break;
        }
    }
    if(it == _childrens.end())
    {
        LOG_ERROR("Write CnfEdit failed,CnfEdit not found.");
        return -1;
    }

    MmsValue *value = MmsValue_newBoolean(confirm);
    auto fullRef = (*it)->getFullRef();
    auto ret = writeData(_conn,fullRef,IEC61850_FC_SP,value);
    if(ret != 0)
    {
        LOG_ERROR("Write CnfEdit failed.");
        return -1;
    }
    setConfirm(confirm);
    MmsValue_delete(value);
    return 0;
}

void Sgcb::setNumOfSg(const uint8_t &numOfSg)
{
    if(_numOfSg != numOfSg)
    {
        _numOfSg = numOfSg;
    }
}

uint8_t Sgcb::getNumOfSg() const
{
    return _numOfSg;
}

void Sgcb::setActSg(const uint8_t &actSg)
{
    if(_actSg != actSg)
    {
        _actSg = actSg;
    }
}

uint8_t Sgcb::getActSg() const
{
    return _actSg;
}

void Sgcb::setEditSg(const uint8_t &editSg)
{
    if(_editSg != editSg)
    {
        _editSg = editSg;
    }
}

uint8_t Sgcb::getEditSg() const
{
    return _editSg;
}

void Sgcb::setConfirm(const bool &confirm)
{
    if(_confirm != confirm)
    {
        _confirm = confirm;
    }
}

bool Sgcb::getConfirm() const
{
    return _confirm;
}

void Sgcb::setActTm(const time_t &actTm)
{
    if(_actTm != actTm)
    {
        _actTm = actTm;
    }
}

time_t Sgcb::getActTm() const
{
    return _actTm;
}

void Sgcb::setActTmUs(const suseconds_t &actTmUs)
{
    if(_actTmUs != actTmUs)
    {
        _actTmUs = actTmUs;
    }
}

suseconds_t Sgcb::getActTmUs() const
{
    return _actTmUs;
}

void Sgcb::setResvTms(const uint16_t &resvTms)
{
    if(_resvTms != resvTms)
    {
        _resvTms = resvTms;
    }
}

uint16_t Sgcb::getResvTms() const
{
    return _resvTms;
}

Sgcbs::Sgcbs(IedConnection conn,INode *model,INode *parent)
    :INode(conn,NODE_SGCBS,parent),
    _model(model)
{
    _vecSg.clear();
}

Sgcbs::~Sgcbs()
{
}

int Sgcbs::init()
{
    auto lDas = ((Model*)_model)->getChildrensFromType(NODE_TYPE::NODE_DA);
    for (auto it = lDas.begin(); it != lDas.end(); it++)
    {
        auto lda = (LDa *)(*it);
        if(lda->getFc() == IEC61850_FC_SE)
        {
            _vecSg.emplace_back(lda);
        }
    }

    // SG 状态
    auto lst = ((Model*)_model)->getChildrens();
    for (auto iter = lst.begin();iter != lst.end();iter++)
    {
        auto ld = *iter;
        auto ldlst = ld->getChildrens();
        for(auto it = ldlst.begin();it != ldlst.end();it++)
        {
            auto ln = *it;
            if(ln->getRef() != "LLN0")
            {
                continue;
            }
            // 判断是否存在SGBC
            auto ldo = ln->getSubNodeFromRef("SGCB");
            if(!ldo)
            {
                continue;
            }
            auto sgcb = new Sgcb(_conn);
            sgcb->setRef(ln->getFullRef() + ".SGCB");
            sgcb->setChildrens(ldo->getChildrens());
            if(sgcb->init() == -1)
            {
                LOG_ERROR("Init sgcb failed.");
                return -1;
            }
            _childrens.push_back(sgcb);
            break;
        }
    }
    return 0;
}

int Sgcbs::readSGCB(const std::string &ref,MmsValue *value)
{
    if(_vecSg.empty())
    {
        return -1;
    }
    auto isExist = false;
    for (auto it = _vecSg.begin(); it != _vecSg.end(); it++)
    {
        auto sg = *it;
        if(sg->getFullRef() == ref)
        {
            isExist = !isExist;
            break;
        }
    }
    
    if (!isExist)
    {
        return -1;
    }
    auto sgcb = (Sgcb *)getSgcbFromRef(ref);
    if(!sgcb)
    {
        return -1;
    }
    return 0;
}

int Sgcbs::writeSGCB(const std::string &ref,MmsValue *value,const uint8_t &num)
{
    int result = 0;
    if(_vecSg.empty())
    {
        LOG_ERROR("Sgcb list is null.");
        return -1;
    }

    auto isExist = false;
    for (auto it = _vecSg.begin(); it != _vecSg.end(); it++)
    {
        auto sg = *it;
        if(sg->getFullRef() == ref)
        {
            isExist = !isExist;
            break;
        }
    }
    
    if (!isExist)
    {
        LOG_ERROR("Not found write sgcb, the ref is : [ " + ref + " ]");
        return -1;
    }
    auto sgcb = (Sgcb *)getSgcbFromRef(ref);
    if(!sgcb)
    {
        LOG_ERROR("sgcb object is null");
        return -1;
    }
    // 重新读SG属性
    if(sgcb->update() != 0)
    {
        LOG_ERROR("Update sgcb property failed.");
        return -1;
    }
    // 设置需要写入的组
    if(sgcb->writeEditSg(num) != 0)
    {
        LOG_ERROR("Write EditSg failed, the ref is : [ " + ref + " ]");
        return -1;
    }

    if(writeData(_conn,ref,IEC61850_FC_SE,value) != 0)
    {
        LOG_ERROR("Write SGCB value failed, the ref is : [ " + ref + " ]");
        return -1;
    }
    if(sgcb->writeConfirm(true) != -0)
    {
        LOG_ERROR("Write CnfEdit value failed, the ref is : [ " + ref + " ]");
        return -1;
    }
    if(sgcb->writeEditSg(0) != -0)
    {
        LOG_ERROR("Write EditSg failed, the ref is : [ " + ref + " ]");
        return -1;
    }
    return 0;
}

INode *Sgcbs::getSgcbFromRef(const std::string &ref)
{
    auto ldRef = ref.substr(0,ref.find_first_of("/"));

    auto lst = getChildrens();
    auto it = lst.begin();
    for (; it != lst.end(); it++)
    {
        auto sgcb = *it;
        auto fullRef = sgcb->getRef();
        if(strstr(fullRef.c_str(),ldRef.c_str()))
        {
            break;
        }
    }
    if(it != lst.end())
    {
        return *it;
    }
    else
    {
        return nullptr;
    }
}
