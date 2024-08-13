#include "reports.h"
#include <functional>
#include "model.h"
#include "dataset.h"
#include "internal/units.h"
#include "internal/log.h"

ReportResult::ReportResult(ClientReport report)
    :_hasTimeStamp(false),
    _hasSeqNum(false),
    _hasDataSetName(false),
    _hasReasonForInclusion(false),
    _hasConfRev(false),
    _hasBufOvfl(false),
    _hasDataReference(false),
    _hasSubSeqNum(false),
    _datasetName(),
    _rcbRef(),
    _rptId(),
    _seqNum(0),
    _confRev(0),
    _bufOvfl(false),
    _timestamp(0),
    _subSeqNum(0),
    _moreSeqmentsFollow(false),
    _value(nullptr)
{
    if(report)
    {
        init(report);
    }
}

ReportResult::~ReportResult()
{

}

void ReportResult::init(ClientReport report)
{
    _hasTimeStamp = ClientReport_hasTimestamp(report);
    _hasSeqNum = ClientReport_hasSeqNum(report);
    _hasDataSetName = ClientReport_hasDataSetName(report);
    _hasReasonForInclusion = ClientReport_hasReasonForInclusion(report);
    _hasConfRev = ClientReport_hasConfRev(report);
    _hasBufOvfl = ClientReport_hasBufOvfl(report);
    _hasDataReference = ClientReport_hasDataReference(report);
    _hasSubSeqNum = ClientReport_hasSubSeqNum(report);

    _rcbRef = ClientReport_getRcbReference(report);
    _rptId = ClientReport_getRptId(report);

    if(_hasTimeStamp)
    {
        _timestamp = ClientReport_getTimestamp(report);
    }
    if(_hasSeqNum)
    {
        _seqNum = ClientReport_getSeqNum(report);
    }
    if(_hasDataSetName)
    {
        _datasetName = ClientReport_getDataSetName(report);
        stringReplace(_datasetName,"$",".");
    }
    if(_hasConfRev)
    {
        _confRev = ClientReport_getConfRev(report);
    }
    if(_hasBufOvfl)
    {
        _bufOvfl = ClientReport_getBufOvfl(report);
    }
    if(_hasDataReference)
    {
        // _dataReference = std::string(ClientReport_getDataReference(report));
    }
    if(_hasSubSeqNum)
    {
        _subSeqNum = ClientReport_getSubSeqNum(report);
    }

    _moreSeqmentsFollow = ClientReport_getMoreSeqmentsFollow(report);

    _value = ClientReport_getDataSetValues(report);
}

bool ReportResult::hasTimeStamp() const
{
    return _hasTimeStamp;
}

bool ReportResult::hasSeqNum() const
{
    return _hasSeqNum;
}

bool ReportResult::hasDataSetName() const
{
    return _hasDataSetName;
}

bool ReportResult::hasReasonForInclusion() const
{
    return _hasReasonForInclusion;
}

bool ReportResult::hasConfRev() const
{
    return _hasConfRev;
}

bool ReportResult::hasBufOvfl() const
{
    return _hasBufOvfl;
}

bool ReportResult::hasDataReference() const
{
    return _hasDataReference;
}

bool ReportResult::hasSubSeqNum() const
{
    return _hasSubSeqNum;
}

std::string ReportResult::getDatasetName() const
{
    return _datasetName;
}

std::string ReportResult::getRcbRef() const
{
    return _rcbRef;
}

std::string ReportResult::getRptId() const
{
    return _rptId;
}

uint16_t ReportResult::getSeqNum() const
{
    return _seqNum;
}

uint32_t ReportResult::getConfRev() const
{
    return _confRev;
}

bool ReportResult::getBufOvfl() const
{
    return _bufOvfl;
}

uint64_t ReportResult::getTimestamp() const
{
    return _timestamp;
}

uint16_t ReportResult::getSubSeqNum() const
{
    return _subSeqNum;
}

bool ReportResult::getMoreSeqmentsFollow() const
{
    return _moreSeqmentsFollow;
}

MmsValue *ReportResult::getValue() const
{
    return _value;
}

Rcb::Rcb(IedConnection conn,const NODE_TYPE &type,INode *parent)
    :INode(conn,type,parent),
    _enabledBySelf(false)
{

}

Rcb::~Rcb()
{
}

int Rcb::enable(ReportCallbackFunction handler, void *handlerParameter,const int optflds, const int trgops, const std::string& entryid)
{
    IedClientError err;
    auto ref = getFullRef();
    auto rcb = IedConnection_getRCBValues(_conn,&err,_ref.c_str(),NULL);
    if(err != IED_ERROR_OK || !rcb)
    {
        LOG_ERROR("IedConnection_getRCBValues failed.the error code is : [ " + std::to_string(err) + " ].");
        return -1;
    }
    
    if(readProperty(rcb) != 0)
    {
        LOG_ERROR("Read property failed.");
        return -1;
    }
    
    auto ena = ClientReportControlBlock_getRptEna(rcb);
    if(ena)
    {
        ClientReportControlBlock_destroy(rcb);
        return -1;
    }
    std::string rptid = _ref.c_str();

    if(handler)
    {
        LOG_INFO("Enable RCB client report callback.");
        IedConnection_installReportHandler(_conn,ref.c_str(),rptid.c_str(),handler,handlerParameter);
    }
    else
    {
        LOG_INFO("report callback function is null.");
        return -1;
    }
    
    uint32_t mask = RCB_ELEMENT_RPT_ENA | RCB_ELEMENT_RPT_ID;

    ClientReportControlBlock_setRptId(rcb, rptid.c_str());

    if (optflds != 0)
    {
        mask |= RCB_ELEMENT_OPT_FLDS;
        ClientReportControlBlock_setOptFlds(rcb, optflds);
    }
    if (trgops != 0)
    {
        mask |= RCB_ELEMENT_TRG_OPS;
        ClientReportControlBlock_setTrgOps(rcb, trgops);
    }
    if (!entryid.empty())
    {
        mask |= RCB_ELEMENT_ENTRY_ID;
        MmsValue* v = MmsValue_newOctetString(0, entryid.size());
        MmsValue_setOctetString(v, (uint8_t*)entryid.data(), entryid.size());
        ClientReportControlBlock_setEntryId(rcb, v);
        MmsValue_delete(v);
    }
    ClientReportControlBlock_setRptEna(rcb, true);
    IedConnection_setRCBValues(_conn, &err, rcb, mask, true);
    if (err != IED_ERROR_OK)
    {
        LOG_ERROR("IedConnection_setRCBValues failed.the error code is : " + std::to_string(err) + " ].");
        IedConnection_uninstallReportHandler(_conn, ref.c_str());
        ClientReportControlBlock_destroy(rcb);
        return -1;
    }

    _enabledBySelf = true;
    return 0;
}

int Rcb::disable()
{
    IedClientError err;
    auto ref = getFullRef();
    LOG_INFO("Disable RCB client report callback.");
    IedConnection_uninstallReportHandler(_conn,ref.c_str());
    auto rcb = IedConnection_getRCBValues(_conn,&err,ref.c_str(),NULL);
    if (err != IED_ERROR_OK || !rcb)
    {
        LOG_ERROR("IedConnection_getRCBValues failed. the error code is : [ " + std::to_string(err) + " ]");
        return -1;
    }

    bool enable = ClientReportControlBlock_getRptEna(rcb);
    if(!enable)
    {
        ClientReportControlBlock_destroy(0);
        return 0;
    }
    else
    {
        ClientReportControlBlock_setRptEna(rcb,false);
        IedConnection_setRCBValues(_conn,&err,rcb,RCB_ELEMENT_RPT_ENA,true);
        if (err != IED_ERROR_OK)
        {
            ClientReportControlBlock_destroy(rcb);
            return -1;
        }
    }
    writeProperty(rcb);
    setEnableBySelf(false);
    return 0;
}

void Rcb::setBuffered(const bool &buffered)
{
    if(_buffered != buffered)
    {
        _buffered = buffered;
    }
}

bool Rcb::getBuffered() const
{
    return _buffered;
}

void Rcb::setRptId(const std::string &rptId)
{
    if(_rptId != rptId)
    {
        _rptId = rptId;
    }
}

std::string Rcb::getRptId() const
{
    return _rptId;
}

void Rcb::setEnable(const bool &enable)
{
    if(_enable != enable)
    {
        _enable = enable;
    }
}

bool Rcb::getEnable() const
{
    return _enable;
}

void Rcb::setDataset(const std::string &dataset)
{
    if(_dataset != dataset)
    {
        _dataset = dataset;
    }
}

std::string Rcb::getDataset() const
{
    return _dataset;
}

uint32_t Rcb::getConfRev() const
{
    return _confRev;
}

void Rcb::setOptflds(const int &optflds)
{
    if(_optflds != optflds)
    {
        _optflds = optflds;
    }
}

int Rcb::getOptflds() const
{
    return _optflds;
}

void Rcb::setBufTm(const uint32_t &bufTm)
{
    if(_bufTm != bufTm)
    {
        _bufTm = bufTm;
    }
}

uint32_t Rcb::getBufTm() const
{
    return _bufTm;
}

uint16_t Rcb::getSqnum() const
{
    return _sqnum;
}

void Rcb::setTrgops(const int &trgops)
{
    if(_trgops != trgops)
    {
        _trgops = trgops;
    }
}

int Rcb::getTrgops() const
{
    return _trgops;
}

void Rcb::setIntgPd(const uint32_t &intgPd)
{
    if(_intgPd != intgPd)
    {
        _intgPd = intgPd;
    }
}

uint32_t Rcb::getIntgPd() const
{
    return _intgPd;
}

void Rcb::setGi(const bool &gi)
{
    if(_gi != gi)
    {
        _gi = gi;
    }
}

bool Rcb::getGi() const
{
    return _gi;
}

std::string Rcb::getOwner() const
{
    return _owner;
}

void Rcb::setLastEntryId(const std::string &lastEntryId)
{
    if(_lastEntryId != lastEntryId)
    {
        _lastEntryId = lastEntryId;
    }
}

std::string Rcb::getLastEntryId() const
{
    return _lastEntryId;
}

void Rcb::setEnableBySelf(const bool &enableBySelf)
{
    if(_enabledBySelf != enableBySelf)
    {
        _enabledBySelf = enableBySelf;
    }
}

bool Rcb::getEnableBySelf() const
{
    return _enabledBySelf;
}

int Rcb::readProperty(ClientReportControlBlock rcb)
{
    if(!rcb)
        return -1;

    _ref = ClientReportControlBlock_getObjectReference(rcb);

    _buffered = ClientReportControlBlock_isBuffered(rcb);
    
    _rptId = ClientReportControlBlock_getRptId(rcb);

    _enable = ClientReportControlBlock_getRptEna(rcb);

    // 需要将"$" --->  "."
    _dataset = ClientReportControlBlock_getDataSetReference(rcb);
    stringReplace(_dataset,"$",".");

    _confRev = ClientReportControlBlock_getConfRev(rcb);

    _optflds = ClientReportControlBlock_getOptFlds(rcb);

    _bufTm = ClientReportControlBlock_getBufTm(rcb);

    _sqnum = ClientReportControlBlock_getSqNum(rcb);

    _trgops = ClientReportControlBlock_getTrgOps(rcb);

    _intgPd = ClientReportControlBlock_getIntgPd(rcb);

    _gi = ClientReportControlBlock_getGI(rcb);

    uint64_t ms = ::ClientReportControlBlock_getEntryTime(rcb);

    auto owner = ::ClientReportControlBlock_getOwner(rcb);
    if (owner != NULL && ::MmsValue_getOctetStringSize(owner) > 0)
        _owner = std::string((const char*)::MmsValue_getOctetStringBuffer(owner),MmsValue_getOctetStringSize(owner));
    else
        _owner.clear();

    return 0;
}

int Rcb::writeProperty(ClientReportControlBlock rcb)
{
    if (rcb == NULL)
        return -1;
    ClientReportControlBlock_setRptId(rcb, _rptId.c_str());
    ClientReportControlBlock_setRptEna(rcb, _enable);
    std::string dsref = _dataset;
    stringReplace(dsref, ".", "$");
    ClientReportControlBlock_setDataSetReference(rcb, dsref.c_str());
    ClientReportControlBlock_setOptFlds(rcb, _optflds);
    ClientReportControlBlock_setBufTm(rcb, _bufTm);
    ClientReportControlBlock_setTrgOps(rcb, _trgops);
    ClientReportControlBlock_setIntgPd(rcb, _intgPd);
    ClientReportControlBlock_setGI(rcb, _gi);
    return 0;
}

Brcb::Brcb(IedConnection conn,INode *parent)
    :Rcb(conn,NODE_BRCB,parent)
{
}

Brcb::~Brcb()
{
}

int Brcb::init()
{
    return 0;
}

void Brcb::setPurgeBuf(const bool &purgeBuf)
{
    if(_purgeBuf != purgeBuf)
    {
        _purgeBuf = purgeBuf;
    }
}

bool Brcb::getPurgeBuf() const
{
    return _purgeBuf;
}

void Brcb::setEntryId(const std::string &entryId)
{
    if(_entryId != entryId)
    {
        _entryId = entryId;
    }
}

std::string Brcb::getEntryId() const
{
    return _entryId;
}

void Brcb::setTimeOfEntry(const time_t &timeOfEntry)
{
    if(_timeOfEntry != timeOfEntry)
    {
        _timeOfEntry = timeOfEntry;
    }
}

time_t Brcb::getTimeOfEntry() const
{
    return _timeOfEntry;
}

void Brcb::setTimeOfEntryUs(const suseconds_t &timeOfEntryUs)
{
    if(_timeOfEntryUs != timeOfEntryUs)
    {
        _timeOfEntryUs = timeOfEntryUs;
    }
}

suseconds_t Brcb::getTimeOfEntryUs() const
{
    return _timeOfEntryUs;
}

void Brcb::setResvTms(const int16_t &resvTms)
{
    if(_resvTms != resvTms)
    {
        _resvTms = resvTms;
    }
}

int16_t Brcb::getResvTms() const
{
    return _resvTms;
}

int Brcb::readProperty(ClientReportControlBlock rcb)
{
    if(Rcb::readProperty(rcb) == -1)
    {
        LOG_ERROR("Read property failed.");
        return -1;
    }
    _purgeBuf = ClientReportControlBlock_getPurgeBuf(rcb);
    auto entryid = ClientReportControlBlock_getEntryId(rcb);
    if (entryid != NULL && ::MmsValue_getOctetStringSize(entryid) > 0)
        _entryId = std::string((const char*)::MmsValue_getOctetStringBuffer(entryid),MmsValue_getOctetStringSize(entryid));
    else
        _entryId.clear();

    uint64_t ms = ::ClientReportControlBlock_getEntryTime(rcb);
    _timeOfEntry = (time_t)(ms / 1000);
    _timeOfEntryUs = (suseconds_t)(ms % 1000) * 1000;
    _resvTms = ClientReportControlBlock_getResvTms(rcb);
    return 0;
}

int Brcb::writeProperty(ClientReportControlBlock rcb)
{
    if(Rcb::writeProperty(rcb) == -1)
    {
        LOG_ERROR("Write property failed.");
        return -1;
    }
    ClientReportControlBlock_setResvTms(rcb,_resvTms);
    ClientReportControlBlock_setPurgeBuf(rcb, _purgeBuf);
    MmsValue* entryid = MmsValue_newOctetString(0, _entryId.size());
    MmsValue_setOctetString(entryid, (uint8_t*)this->_entryId.data(), _entryId.size());
    ClientReportControlBlock_setEntryId(rcb, entryid);
    MmsValue_delete(entryid);
    return 0;
}

Urcb::Urcb(IedConnection conn,INode *parent)
    :Rcb(conn,NODE_URCB,parent)
{
}

Urcb::~Urcb()
{
}

int Urcb::init()
{
    return 0;
}


void Urcb::setResv(const bool &resv)
{
    if(_resv != resv)
    {
        _resv = resv;
    }
}

bool Urcb::getResv() const
{
    return _resv;
}

int Urcb::readProperty(ClientReportControlBlock rcb)
{
    if(Rcb::readProperty(rcb) == -1)
    {
        LOG_ERROR("Read property failed");
        return -1;
    }

    _resv = ClientReportControlBlock_getResv(rcb);
    return 0;
}

int Urcb::writeProperty(ClientReportControlBlock rcb)
{
    if(Rcb::writeProperty(rcb) == -1)
    {
        LOG_ERROR("Write property failed");
        return -1;
    }

    ClientReportControlBlock_setResv(rcb, _resv);
    return 0;
}

Reports::Reports(IedConnection conn,INode *model)
    :INode(conn,NODE_INVALID,nullptr),
    _model(model)
{
}

Reports::~Reports()
{
}

int Reports::init()
{
    auto ret = 0;
    auto lst = ((Model*)_model)->getChildrens();
    for (auto it = lst.begin();it != lst.end();it++)
    {
        auto ld = *it;
        if(!ld)
        {
            LOG_ERROR("LD is null");
            return -1;
        }
        auto lnlst = ld->getChildrens();
        for(auto it2 = lnlst.begin();it2 != lnlst.end();it2++)
        {
            auto ln = *it2;
            if(initBrcb(ln) == -1)
            {
                ret = -1;
                LOG_ERROR("Init brcb failed.");
                break;
            }
            if(initUrcb(ln) == -1)
            {
                ret = -1;
                LOG_ERROR("Init urcb failed.");
                break;
            }
        }
    }
    return ret;
}

int Reports::enableBrcb(ReportCallbackFunction handler, void *handlerParameter,const int optflds, const int trgops, const std::string& entryid)
{
    bool isEnable = false;
    for(auto it = _childrens.begin(); it != _childrens.end();it++)
    {
        Brcb *brcb = (Brcb*)(*it);
        if(brcb->getType() == NODE_BRCB)
        {
            // 判断当前Brcb是否已经被占用
            if(brcb->getEnable())
            {
                continue;
            }
            if(brcb->enable(handler,handlerParameter,optflds,trgops,entryid) != 0)
            {
                continue;
            }
            else
            {
                isEnable = !isEnable;
                break;
            }
        }
    }
    return isEnable ? 0 : -1;
}

int Reports::enableUrcb(ReportCallbackFunction handler, void *handlerParameter,const int optflds, const int trgops, const std::string& entryid)
{
    bool isEnable = false;
    for(auto it = _childrens.begin(); it != _childrens.end();it++)
    {
        Urcb *urcb = (Urcb*)(*it);
        if(urcb->getType() == NODE_URCB)
        {
            if(urcb->getEnable())
            {
                continue;
            }
            if(urcb->enable(handler,handlerParameter,optflds,trgops,entryid) != 0)
            {
                continue;
            }
            else
            {
                isEnable = !isEnable;
                break;
            }
        }
    }
    return isEnable ? 0 : -1;
}

int Reports::disableBrcb()
{
    auto ret = 0;
    for (auto it = _childrens.begin(); it != _childrens.end(); it++)
    {
        Brcb *brcb = (Brcb*)(*it);
        if(brcb->getType() == NODE_BRCB && brcb->getEnableBySelf())
        {
            if(brcb->disable() == -1)
            {
                return -1;
            }
            break;
        }
    }
    return 0;
}

int Reports::disableUrcb()
{
    auto ret = 0;
    for (auto it = _childrens.begin(); it != _childrens.end(); it++)
    {
        Urcb *urcb = (Urcb*)(*it);
        if(urcb->getType() == NODE_BRCB && urcb->getEnableBySelf())
        {
            if(urcb->disable() == -1)
            {
                return -1;
            }
            break;
        }
    }
    return 0;
}

int Reports::initBrcb(INode *ln)
{
    IedClientError err;
    LinkedList list = ::IedConnection_getLogicalNodeDirectory(_conn,&err, ln->getFullRef().c_str(), ACSI_CLASS_BRCB);
    if (err != IED_ERROR_OK)
    {
        LOG_ERROR("IedConnection_getLogicalNodeDirectory failed.the error code is : [ " + std::to_string(err) + " ]");
        if (list != NULL)
            ::LinkedList_destroy(list);
        return -1;
    }

    if (list == NULL)
    {
        LOG_ERROR("IedConnection_getLogicalNodeDirectory failed.the error code is : [ " + std::to_string(err) + " ]");
        return -1;
    }

    int ret = 0;
    LinkedList item = ::LinkedList_getNext(list);
    while (item != NULL)
    {
        auto brcb = new Brcb(_conn,this);
        auto ref = ln->getFullRef() + ".BR." +(const char *)item->data;
        brcb->setRef(ref);
        if(brcb->init() == -1)
        {
            LOG_ERROR("Init Brcb fialed.");
            ret = -1;
            break;
        }
        _childrens.push_back(brcb);
        item = ::LinkedList_getNext(item);
    }
    LinkedList_destroy(list);
    return ret;
}

int Reports::initUrcb(INode *ln)
{
    IedClientError err;
    LinkedList list = ::IedConnection_getLogicalNodeDirectory(_conn, &err, ln->getFullRef().c_str(), ACSI_CLASS_URCB);
    if (err != IED_ERROR_OK)
    {
        LOG_ERROR("IedConnection_getLogicalNodeDirectory failed.the error code is : [ " + std::to_string(err) + " ]");
        if (list != NULL)
            ::LinkedList_destroy(list);
        return -1;
    }

    if (list == NULL)
    {
        LOG_ERROR("IedConnection_getLogicalNodeDirectory failed,getLogicalNodeDirectory is null");
    }

    int ret = 0;
    LinkedList item = ::LinkedList_getNext(list);
    while (item != NULL)
    {
        auto urcb = new Urcb(_conn,this);
        auto ref = ln->getFullRef() + ".RP." +(const char *)item->data;
        urcb->setRef(ref);
        if(urcb->init() == -1)
        {
            LOG_ERROR("Init URCB failed.");
            ret = -1;
            break;
        }
        _childrens.push_back(urcb);
        item = ::LinkedList_getNext(item);
    }
    LinkedList_destroy(list);
    return ret;
}