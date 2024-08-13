/*
 * @Author: your name
 * @Date: 2021-07-04 19:24:02
 * @LastEditTime: 2021-07-04 19:29:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /iec61850/src/gwmanager/gwdata.h
 */
#ifndef __IEC_GW_DATA_H__
#define __IEC_GW_DATA_H__

#include <atomic>
#include <vector>
#include <functional>
#include <json/value.h>
#include "../database.h"

using ReportSwitchCallback = std::function<int(void)>;
using SetControlCallback = std::function<int (const std::string &,MmsValue *,const uint8_t &)>;

const std::string SUBSCRIBE_CONTROL_CMD = "/gwmanager/61850client/command";
const std::string PUBLISH_CONTROL_CMD = "/61850client/gwmanager/reply";
const std::string PUBLISH_REPORT_TOPIC = "/61850client/gwmanager/upload";
const std::string SUBSCRIBE_GET_DEVICE_LIST_RESPONSE = "/gwmanager/61850client/response";
const std::string PUBLISH_GET_DEVICE_LIST_REQUEST = "/61850client/gwmanager/request";

class GWData : public virtual IDataConnect , public virtual IDataReport , public virtual IDataSet
{
public:
    struct Device
    {
        std::string deviceId;
        std::string sensorId;
        std::string model;
        Json::Value val;
    };
public:
    GWData();
    ~GWData();
public:
    void start();
    bool isInit();
    void wait();
    //61850
    virtual void enable();
    virtual void disable();
    void registerEnable(ReportSwitchCallback enableCallback);
    void registerDisable(ReportSwitchCallback disableCallback);
    virtual int setcontrolValue(const std::string &ref,MmsValue *value,const uint8_t &num);
    void registerSetControlValue(SetControlCallback callback);

    void onConnectedChanged(const int &status);

    void onReportArrived(const std::unordered_map<std::string ,std::string> &maps) final;
    // mqtt
    void onControlCmdArrived(const std::string &msg);
protected:
    void initMqttSubscribeCallback();
private:
    std::atomic<bool> _initStatus;

    ReportSwitchCallback _enableCallback;
    ReportSwitchCallback _disableCallback;
    SetControlCallback _setControlCallback;
};

#endif // ! __IEC_GW_DATA_H__