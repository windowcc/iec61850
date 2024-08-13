#include "client.h"
#include <thread>
#include <unordered_map>
#include <json/json.h>
#include "global.h"
#include "model.h"
#include "internal/log.h"

static void RcbReportCallback(void* parameter, ClientReport report);

Iec61850Client::Iec61850Client(const ConnectionParam &param)
    :_param(param),
    _model(nullptr),
    _dataSets(nullptr),
    _reports(nullptr),
    _sgcbs(nullptr),
    _enable(false),
    _conn(nullptr)
{
    if(!GwData)
        return;
    using namespace std::placeholders;
    GwData->registerEnable(std::bind(&Iec61850Client::enbale,this));
    GwData->registerDisable(std::bind(&Iec61850Client::disable,this));
    GwData->registerSetControlValue(std::bind(&Iec61850Client::writeSGCB,this,_1,_2,_3));
    _connectCallback = std::bind(&GWData::onConnectedChanged,GwData,_1);
    _reportCallback = std::bind(&GWData::onReportArrived,GwData,_1);
}

Iec61850Client::~Iec61850Client()
{
    uninit();
}

int Iec61850Client::connect(const bool &async)
{
    IedClientError err = IED_ERROR_OK;
    if(!_conn)
    {
        _conn = IedConnection_create();
    }
    IedConnection_installStateChangedHandler(_conn, [](void * parameter,
        IedConnection connection,IedConnectionState newState)->void
        {
            Iec61850Client *client = (Iec61850Client *)parameter;
            switch (newState)
            {
            case IED_STATE_CLOSED:
                LOG_INFO("Connection { " + client->_param.ip_  + "} closed.");
                //client->setConnectStatus(true);
                //client->notify_one();
                break;
            case IED_STATE_CONNECTING:
                LOG_INFO("Connection { " + client->_param.ip_  + "} connecting.");
                break;

            case IED_STATE_CONNECTED:
                LOG_INFO("Connection { " + client->_param.ip_  + "} connected.");
                break;

            case IED_STATE_CLOSING:
                LOG_INFO("Connection { " + client->_param.ip_  + "} closing.");
                break;
            default:
                LOG_WARN("Invalid connection state.");
                break;
            }
        }, this);

    //IedConnection_installConnectionClosedHandler(IedConn,ConnectionCloseCallback,this);
    IedConnection_setConnectTimeout(_conn, _param.connect_timeout_);
    IedConnection_setRequestTimeout(_conn, _param.request_timeout_);

    if (async)
    {
        IedConnection_connectAsync(_conn,&err, _param.ip_.c_str(), _param.port_);
        if (err != IED_ERROR_OK)
        {
            LOG_ERROR("Connection 61850 server failed,the error code is : " + std::to_string(err));
            return -1;
        }
    }
    else
    {
        IedConnection_connect(_conn,&err, _param.ip_.c_str(), _param.port_);
        if (err != IED_ERROR_OK)
        {
            LOG_ERROR("Connection 61850 server failed,the error code is : " + std::to_string(err));
            return -1;
        }
    }
    return 0;
}

int Iec61850Client::reconnect()
{
    if(uninit() != 0)
    {
        LOG_ERROR("uninit failed.");
    }
    return connect();
}

int Iec61850Client::disconnect()
{
    if (_conn)
    {
        IedClientError err = IED_ERROR_OK;
        IedConnection_release(_conn, &err);
        if (err != IED_ERROR_OK)
        {
            LOG_ERROR("Close 61850 client failed,the clientId is : [ " + _param.name_ + " ]");
        }
        IedConnection_close(_conn);
        _conn = nullptr;
    }

    return 0;
}

int Iec61850Client::init()
{
    if(!_conn)
    {
        LOG_ERROR("61850 client object is null");
        return -1;
    }

    IedClientError err;
    IedConnection_getDeviceModelFromServer(_conn,&err);
    if(err != IED_ERROR_OK)
    {
        LOG_ERROR("IED connection get device model from server failed, the error code is : [ " + std::to_string(err) + " ]");
        return -1;
    }

    if(initModel() == -1)
    {
        LOG_ERROR("Init model failed.");
        return -1;
    }

    if(initDatsets() == -1)
    {
        LOG_ERROR("Init DataSets failed.");
        return -1;
    }
    if(initReports() == -1)
    {
        LOG_ERROR("Init Reports failed.");
        return -1;
    }
    if(initSgcbs() == -1)
    {
        LOG_ERROR("Init SGCBS failed.");
        return -1;
    }
    if(_connectCallback)
        _connectCallback(0);
    return 0;
}

int Iec61850Client::uninit()
{
    if(_connectCallback)
        _connectCallback(1);
    if(_conn)
    {
        IedConnection_destroy(_conn);
        _conn = nullptr;
    }
    if(_dataSets)
    {
        delete _dataSets;
        _dataSets = nullptr;
    }
    if(_reports)
    {
        delete _reports;
        _reports = nullptr;
    }
    if(_sgcbs)
    {
        delete _sgcbs;
        _sgcbs = nullptr;
    }
    if(_model)
    {
        delete _model;
        _model = nullptr;
    }
    // DESTORY_SINGLETON(Model);
    return 0;
}

void Iec61850Client::loop()
{
    while (true)
    {
        auto status = IedConnection_getState(_conn);
        if(status == IedConnectionState::IED_STATE_CLOSED)
        {
            LOG_INFO("61850 reconnect");
            if(reconnect() != 0)
            {
                LOG_INFO("Reconnect failed.");
            }
            else
            {
                init();
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(_param.reconnect_seconds_));
    }
}

INode *Iec61850Client::getMode() const
{
    return _model;
}

int Iec61850Client::enbale()
{
    auto ret = 0;
    if(!_reports)
    {
        LOG_ERROR("The data report object is empty");
        return -1;
    }
    ret = _reports->enableBrcb(RcbReportCallback,this);
    if(ret != 0)
    {
        LOG_ERROR("Enable brcb report failed.");
    }

    ret = _reports->enableUrcb(RcbReportCallback,this);
    if(ret != 0)
    {
        LOG_ERROR("Enable urcb report failed.");
    }
    _enable.store(true);
    return ret;
}

int Iec61850Client::disable()
{
    auto ret = 0;
    if(!_reports)
    {
        LOG_ERROR("The data report object is empty");
        return -1;
    }
    ret = _reports->disableBrcb();
    if(ret != 0)
    {
        LOG_ERROR("Disable brcb report failed.");
    }
    ret = _reports->disableUrcb();
    if(ret)
    {
        LOG_ERROR("Disable urcb report failed.");
    }
    _enable.store(false);
    return 0;
}

void Iec61850Client::report(const std::unordered_map<std::string ,std::string> &maps)
{
    if(_reportCallback)
        _reportCallback(maps);
}

int Iec61850Client::readSGCB(const std::string &ref,MmsValue *value)
{
    return _sgcbs->readSGCB(ref,value);
}

int Iec61850Client::writeSGCB(const std::string &ref,MmsValue *value,const uint8_t &num)
{
    return _sgcbs->writeSGCB(ref,value,num);
}

Datasets * Iec61850Client::getDatasets() const
{
    return _dataSets;
}

Reports *Iec61850Client::getReports() const
{
    return _reports;
}

ConnectionParam Iec61850Client::getConnectionParam() const
{
    return _param;
}

int Iec61850Client::initModel()
{
    if(!_model)
    {
        _model = new Model(_conn);
    }
    return _model->init();
}

int Iec61850Client::initDatsets()
{
    if(!_model)
    {
        LOG_ERROR("61850 client model is null.");
        return -1;
    }
    if(!_dataSets)
    {
        _dataSets = new Datasets(_conn,_model);
    }
    return _dataSets->init();
}

int Iec61850Client::initReports()
{
    if(!_model)
    {
        LOG_ERROR("61850 client model is null.");
        return -1;
    }
    if(!_reports)
    {
        _reports = new Reports(_conn,_model);
    }
    return _reports->init();
}

int Iec61850Client::initSgcbs()
{
    if(!_model)
    {
        LOG_ERROR("61850 client model is null.");
        return -1;
    }
    if(!_sgcbs)
    {
        _sgcbs = new Sgcbs(_conn,_model);
    }
    return _sgcbs->init();
}

static std::string MmsValue2String(const MmsValue *value,const uint32_t &maxsize = 1024)
{
    if(!value)
        return std::string();
    std::string str(maxsize, '\0');
    ::MmsValue_printToBuffer(value, (char*)str.data(), str.size());
    str.resize(strlen(str.c_str()));
    return str;
}

static std::string MmsValueType(const MmsValue *value)
{
    if(!value)
        return std::string();
    auto type = MmsValue_getType(value);
    std::string typeStr = "ACCESS_ERROR";
    switch (type)
    {
    case 0: typeStr = "ARRAY"; break;
    case 1: typeStr = "STRUCTURE"; break;
    case 2: typeStr = "BOOLEAN"; break;
    case 3: typeStr = "BIT_STRING"; break;
    case 4: typeStr = "INTEGER"; break;
    case 5: typeStr = "UNSIGNED"; break;
    case 6: typeStr = "FLOAT"; break;
    case 7: typeStr = "OCTET_STRING"; break;
    case 8: typeStr = "VISIBLE_STRING"; break;
    case 9: typeStr = "GENERALIZED_TIME"; break;
    case 10: typeStr = "BINARY_TIME"; break;
    case 11: typeStr = "BCD"; break;
    case 12: typeStr = "OBJ_ID"; break;
    case 13: typeStr = "STRING"; break;
    case 14: typeStr = "UTC_TIME"; break;
    case 15: typeStr = "ACCESS_ERROR"; break;
    default:
        break;
    }
    return typeStr;
}

static Json::Value GetChildrenMmsValue(INode *node,MmsValue *value,FunctionalConstraint fc)
{
    Json::Value root;
    auto leafs = node->getChildrens();
    auto ref = node->getFullRef();
    if(leafs.empty())
    {
        auto subRef = ref.substr(ref.find_last_of(".") + 1);
        if(subRef == "stVal" || subRef == "f" )
        {
            auto key = node->getRef();
            root[key] = MmsValue2String(value);
        }
        else if(subRef == "t")
        {
            /**
             * bit 7 = leapSecondsKnown
             * bit 6 = clockFailure
             * bit 5 = clockNotSynchronized
             * bit 0-4 = subsecond time accuracy (number of significant bits of subsecond time)
             */
            auto timeQuality = MmsValue_getUtcTimeQuality(value);
            auto key = node->getRef();
            root[key]["timestamp"] = MmsValue2String(value);
            root[key]["LeapSecondsKnown"] = (timeQuality & 0x80) > 7;
            root[key]["ClockFailure"] = (timeQuality & 0x64) > 6;
            root[key]["ClockNotSynchronized"] = (timeQuality & 0x32) > 5;
            root[key]["TimeAccuracy"] = (timeQuality & 0x0F);
        }
        else if(subRef == "q" && (MmsType::MMS_BIT_STRING == MmsValue_getType(value)))
        {
            auto key = node->getRef();
            auto size = MmsValue_getBitStringSize(value);
            int validity = 0;
            auto bitflag = MmsValue_getBitStringBit(value,0);
            if(bitflag)
                validity &= 0x01;
            bitflag = MmsValue_getBitStringBit(value,1);
            if(bitflag)
                validity &= 0x02;
            root[key]["Validity"] = validity;
            root[key]["QualityDetails"]["Overflow"] = MmsValue_getBitStringBit(value,2);
            root[key]["QualityDetails"]["OutOfRange"] = MmsValue_getBitStringBit(value,3);
            root[key]["QualityDetails"]["BadReference"] = MmsValue_getBitStringBit(value,4);
            root[key]["QualityDetails"]["Oscillatory"] = MmsValue_getBitStringBit(value,5);   
            root[key]["QualityDetails"]["Failure"] = MmsValue_getBitStringBit(value,6);
            root[key]["QualityDetails"]["OldData"] = MmsValue_getBitStringBit(value,7);
            root[key]["QualityDetails"]["Inconsistent"] = MmsValue_getBitStringBit(value,8);
            root[key]["QualityDetails"]["Inaccurate"] = MmsValue_getBitStringBit(value,9);
            root[key]["Source"] = MmsValue_getBitStringBit(value,10) ? "substituted" : "process";
            root[key]["Test"] = MmsValue_getBitStringBit(value,11);
            root[key]["OperatorBlocked"] = MmsValue_getBitStringBit(value,12);
        }
    }
    else
    {
        int i = 0;
        for (auto it = leafs.begin(); it != leafs.end(); it++)
        {
            auto lda = (LDa *)(*it);
            if(fc != lda->getFc())
                continue;
            auto key = lda->getRef();
            auto childValue = GetChildrenMmsValue(*it,MmsValue_getElement(value,i++),fc);
            Json::Value::Members mem = childValue.getMemberNames();
            if(mem.size() > 0 && key == mem[0])
                root[key] = childValue[key];
            else
                root[key] = childValue;
        }
    }
    return root;
}

void RcbReportCallback(void* parameter, ClientReport report)
{
    Json::Value root;
    // !   需要注意FCDA中是否存在DONAME 和 DANAME
    Iec61850Client * client = (Iec61850Client *)parameter;

    root["id"] = client->getConnectionParam().name_;

    auto result = std::make_shared<ReportResult>(report);
    auto dataSets = client->getDatasets();
    auto dataSetValues = result->getValue();
    auto dataSetName = result->getDatasetName();
    root["datasetName"] = dataSetName;
    // 通过datasets 找到返回的数据集
    auto dataset = dataSets->getDataset(dataSetName);
    if(!dataset)
    {
        LOG_ERROR("Get Dataset failed.");
        return;
    } 
    
    auto fcdas = dataset->getChildrens();
    if(fcdas.empty())
    {
        LOG_ERROR("Get FCDAs failed.");
        return;
    }
    int i = 0;
    for (auto it = fcdas.begin();it != fcdas.end();it++)
    {
        Fcda *fcda = (Fcda *)*it;
        auto fcdaFc = fcda->getFc();
        auto fcdaRef = fcda->getRef();
        ReasonForInclusion reason = ClientReport_getReasonForInclusion(report, i);
        if(reason != IEC61850_REASON_NOT_INCLUDED && reason != IEC61850_REASON_UNKNOWN)
        {
            auto mmsVal = MmsValue_getElement(dataSetValues, i++);
            auto modelDo = ((Model*)(client->getMode()))->getLeafByRef(fcdaRef,NODE_TYPE::NODE_DO);
            root[fcdaRef.substr(fcdaRef.find("/") + 1)] = GetChildrenMmsValue(modelDo,mmsVal,fcdaFc);
        }
        else
        {
            LOG_WARN("61850 client report reason is : IEC61850_REASON_NOT_INCLUDED or IEC61850_REASON_UNKNOWN");
        }
    }
    LOG_INFO(root.toStyledString());
}