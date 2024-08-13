#include "gwdata.h"
#include <strings.h>
#include <json/json.h>
#include "../global.h"
#include "../internal/log.h"

GWData::GWData()
    :_initStatus(false)
{
}

GWData::~GWData()
{
    // 取消相关topic订阅
    if(!AsyncClient)
    {
    }
    mqtt::token_ptr token;
    using namespace std::placeholders;
    // 订阅
    token = AsyncClient->unsubscribe(SUBSCRIBE_GET_DEVICE_LIST_RESPONSE);
    if(!token->get_reason_code())
    {
    }
    token = AsyncClient->unsubscribe(SUBSCRIBE_CONTROL_CMD);
    if(!token->get_reason_code())
    {
    }
}

void GWData::start()
{
    initMqttSubscribeCallback();
}

bool GWData::isInit()
{
    return _initStatus.load();
}

void GWData::wait()
{
    _initStatus.store(true);
    LOG_DEBUG("wait GWManager init ...");
    while (!_initStatus.load())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void GWData::enable()
{
    if(_enableCallback)
        _enableCallback();
    else
    {
        LOG_ERROR("Report enable function is not bound.");
    }
}

void GWData::disable()
{
    if(_disableCallback)
        _disableCallback();
    else
    {
        LOG_ERROR("Report disable function is not bound.");
    }
}

void GWData::registerEnable(ReportSwitchCallback enableCallback)
{
    _enableCallback = enableCallback;
}

void GWData::registerDisable(ReportSwitchCallback disableCallback)
{
    _disableCallback = disableCallback;
}

int GWData::setcontrolValue(const std::string &ref,MmsValue *value,const uint8_t &num)
{
    if(_setControlCallback)
    {
        return _setControlCallback(ref,value,num);
    }
    else
    {
        LOG_ERROR("Command control function is not bound.");
        return -1;
    }
}

void GWData::registerSetControlValue(SetControlCallback callback)
{
    _setControlCallback = callback;
}

void GWData::onConnectedChanged(const int &status)
{
    LOG_INFO("61850 client status changed,current status is : [ " + std::to_string(status) + " ]");
    if(status == 0)
    {
        enable();
    }
    else
    {
        disable();
    }
}

void GWData::onReportArrived(const std::unordered_map<std::string ,std::string> &maps)
{
    
}

void GWData::onControlCmdArrived(const std::string &msg)
{

}


void GWData::initMqttSubscribeCallback()
{
    if(!AsyncClient)
    {
    }
    mqtt::token_ptr token;
    using namespace std::placeholders;
    token = AsyncClient->subscribe(SUBSCRIBE_CONTROL_CMD,0);
    if(!token->get_reason_code())
    {
        AsyncClient->registerMessageCallback(SUBSCRIBE_CONTROL_CMD,std::bind(&GWData::onControlCmdArrived,this,_1));
    }
}

GWData *GwData = nullptr;