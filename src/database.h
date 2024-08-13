#ifndef __IEC_DATA_BASE_H__
#define __IEC_DATA_BASE_H__

#include <unordered_map>
#include <libiec61850/mms_value.h>

struct IDataConnect
{
    virtual void onConnectedChanged(const int &) = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
};

struct IDataReport
{
    virtual void onReportArrived(const std::unordered_map<std::string ,std::string> &maps) = 0;
};


struct IDataSet
{
    virtual int setcontrolValue(const std::string &ref,MmsValue *value,const uint8_t &num) = 0;
};

struct IDataFile
{
    //virtual int 
};

#endif // ! __IEC_DATA_BASE_H__