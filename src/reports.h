#ifndef __IEC_REPORTS_H__
#define __IEC_REPORTS_H__

#include "node.h"

class DatasetList;

class ReportResult
{
public:
    ReportResult(ClientReport report = nullptr);
    virtual ~ReportResult();
public:
    void init(ClientReport report);

    bool hasTimeStamp() const;
    bool hasSeqNum() const;
    bool hasDataSetName() const;
    bool hasReasonForInclusion() const;
    bool hasConfRev() const;
    bool hasBufOvfl() const;
    bool hasDataReference() const;
    bool hasSubSeqNum() const;

    std::string getDatasetName() const;
    std::string getRcbRef() const;
    std::string getRptId() const;
    uint16_t getSeqNum() const;
    uint32_t getConfRev() const;
    bool getBufOvfl() const;
    uint64_t getTimestamp() const;
    uint16_t getSubSeqNum() const;
    bool getMoreSeqmentsFollow() const;

    MmsValue *getValue() const;
private:
    bool _hasTimeStamp;
    bool _hasSeqNum;
    bool _hasDataSetName;
    bool _hasReasonForInclusion;
    bool _hasConfRev;
    bool _hasBufOvfl;
    bool _hasDataReference;
    bool _hasSubSeqNum;

    std::string _datasetName;
    std::string _rcbRef;
    std::string _rptId;
    uint16_t _seqNum;
    uint32_t _confRev;
    bool _bufOvfl;
    uint64_t _timestamp;
    uint16_t _subSeqNum;
    bool _moreSeqmentsFollow;

    MmsValue *_value;
};


class Rcb : public INode
{
    friend class Brcb;
    friend class Urcb;
public:
    explicit Rcb(IedConnection conn,const NODE_TYPE &type, INode *parent = nullptr);
    virtual ~Rcb();
public:
    virtual int init() = 0;
    int enable(ReportCallbackFunction handler, void *handlerParameter,const int optflds = 0, const int trgops = 0, const std::string& entryid = "");
    int disable();
public:
    void setBuffered(const bool &buffered);
    bool getBuffered() const;
    void setRptId(const std::string &rptId);
    std::string getRptId() const;
    void setEnable(const bool &enable);
    bool getEnable() const;
    void setDataset(const std::string &dataset);
    std::string getDataset() const;
    uint32_t getConfRev() const;
    void setOptflds(const int &optflds);
    int getOptflds() const;
    void setBufTm(const uint32_t &bufTm);
    uint32_t getBufTm() const;
    uint16_t getSqnum() const;
    void setTrgops(const int &trgops);
    int getTrgops() const;
    void setIntgPd(const uint32_t &intgPd);
    uint32_t getIntgPd() const;
    void setGi(const bool &gi);
    bool getGi() const;
    std::string getOwner() const;
    void setLastEntryId(const std::string &lastEntryId);
    std::string getLastEntryId() const;
    void setEnableBySelf(const bool &enableBySelf);
    bool getEnableBySelf() const;
private:
    virtual int readProperty(ClientReportControlBlock rcb);
    virtual int writeProperty(ClientReportControlBlock rcb);
private:
    bool _buffered;
    std::string _rptId;
    bool _enable;               // 当前上报对象是都已经被占用
    std::string _dataset;
    uint32_t _confRev;          // 只读
    int _optflds;
    uint32_t _bufTm;
    uint16_t _sqnum;            //只读
    int _trgops;                //TRG_OPT_DATA_CHANGED, ...
    uint32_t _intgPd;
    bool _gi;
    std::string _owner;         //只读 MMS_OCTET_STRING, 保存存实际的字节串（可能不是可见字符）
    std::string _lastEntryId;   //记录最后一次收到的entryid, 字节串, 不是RCB的一部分

    bool _enabledBySelf;      //报告是否被自身使能
};

class Brcb : public Rcb
{
public:
    explicit Brcb(IedConnection conn,INode *parent = nullptr);
    virtual ~Brcb();
public:
    virtual int init() final;
public:
    void setPurgeBuf(const bool &purgeBuf);
    bool getPurgeBuf() const;
    void setEntryId(const std::string &entryId);
    std::string getEntryId() const;
    void setTimeOfEntry(const time_t &timeOfEntry);
    time_t getTimeOfEntry() const;
    void setTimeOfEntryUs(const suseconds_t &timeOfEntryUs);
    suseconds_t getTimeOfEntryUs() const;
    void setResvTms(const int16_t &resvTms);
    int16_t getResvTms() const;
    int readProperty(ClientReportControlBlock rcb);
    int writeProperty(ClientReportControlBlock rcb);
private:
    bool _purgeBuf;
    std::string _entryId;
    time_t _timeOfEntry;
    suseconds_t _timeOfEntryUs;
    int16_t _resvTms;
};

class Urcb : public Rcb
{
public:
    explicit Urcb(IedConnection conn,INode *parent = nullptr);
    virtual ~Urcb();
public:
    virtual int init() final;
    void setResv(const bool &resv);
    bool getResv() const;
    int readProperty(ClientReportControlBlock rcb);
    int writeProperty(ClientReportControlBlock rcb);
private:
    bool _resv;
};

class Reports : public INode
{
public:
    explicit Reports(IedConnection conn,INode *model);
    virtual ~Reports();
public:
    virtual int init() final;

    int enableBrcb(ReportCallbackFunction handler, void *handlerParameter,const int optflds = 0, const int trgops = 0, const std::string& entryid = "");
    int enableUrcb(ReportCallbackFunction handler, void *handlerParameter,const int optflds = 0, const int trgops = 0, const std::string& entryid = "");

    int disableBrcb();
    int disableUrcb();
private:
    int initBrcb(INode *ln);
    int initUrcb(INode *ln);
private:
    INode *_model;
};

using ReportsPtr = std::shared_ptr<Reports>;

#endif // ! __IEC_REPORTS_H__