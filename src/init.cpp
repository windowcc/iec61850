#include "init.h"
#include <string>
#include <json/json.h>
#include <fstream>
#include <getopt.h>
#include "internal/log.h"


#define IOTGW_VERSION_MAJOR 0
#define IOTGW_VERSION_MINOR 1
#define IOTGW_VERSION_REVERSION 1

static std::string CONFIG_PATH = "/home/ubuntu/workspace/iot/app/iec61850/config/";

// 日志输出名称
std::string log_name;
// 日志实例对象
std::string log_ins;
// 日志可打印级别
// DEBUG
// INFO (默认)
// NOTICE
// WARN
// ERROR
// CRIT
// ALERT
// FATAL
std::string log_level;
// 是否输出到文件中
bool log_tag;

// MQTT
std::string MQTTClientID;
std::string MQTTAddress;
std::string MQTTUsername;
std::string MQTTPasswd;
int MQTTiInterval;
bool MQTTCleanSession;

// 61850
std::string IECName;
std::string IECIp;
int IECPort;
uint32_t IECConnectTimeout;
uint32_t IECRequestTimeout;
int IECReconnectSeconds;

struct option long_options[] =
{
    {"level",1,0,'l'},                  // 日志输出级别
    {"help",0,0,'h'},                   // 帮助信息
    {"version",0,0,'v'}                 // 版本
};

int Init()
{
    if(InitLog() != 0)
    {
        LOG_WARN("The log module failed to initialize, use the default log output.");
    }
    else
    {
        LOG_INFO("The log module is initialized successfully");
    }
    if(InitMQTT() != 0)
    {
        return -1;
    }
    return InitIec61850();
}

int InitLog()
{
    std::string path = CONFIG_PATH + "log.configure";
    Json::Value root;
    std::fstream ifs(path);
    if(ifs.fail())
    {
        return -1;
    }
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &root, &errs))
    {
        return -1;
    }

    log_name = root["log_name"].asString();
    log_ins = root["log_ins"].asString();
    log_level = root["log_level"].asString();
    log_tag = root["log_tag"].asBool();
    return 0;
}

int ShowInfo(int argc, char *argv[])
{
    char ch;
    int index = 0;
    bool flag = false;

    while(((int8_t)(ch = getopt_long(argc, argv, "m:i:p:l:hv",long_options, &index))) != ((int8_t)0xFF))
    {
        switch(ch)
        {
            case 'l':
                log_level = optarg;
                break;
            case 'h':
                ShowHelp();
                return -1;
            case 'v':
                ShowVersion();
                return -1;
            default:
                LOG_ERROR("Invalid parameter symbol.");
                return -1;
        }
    }
    return 0;
}

void ShowHelp()
{
    std::string help =
        std::string("useage : getopt_long [OPTIONS]\n") +
        std::string("option :\n") +
        std::string("-l --level Set output log level,default is [info].\n") +
        std::string("\t <DEBUG> Debug level.\n") +
        std::string("\t <INFO> Info level.\n") +
        std::string("\t <NOTICE> Notice level.\n") +
        std::string("\t <WARN> Warn level.\n") +
        std::string("\t <ERROR> Error level.\n") +
        std::string("\t <CRIT> Crit level.\n") +
        std::string("\t <ALERT> Alert level.\n") +
        std::string("\t <FATAL> Fatal level.\n") +
        std::string("-h --help,Show help documentation and exit.\n") +
        std::string("-v --version,Show version and exit.");
    std::cout << help << std::endl;
}

void ShowVersion()
{
    std::cout << "Version : " << IOTGW_VERSION_MAJOR << "."
        << IOTGW_VERSION_MINOR << "."
        << IOTGW_VERSION_REVERSION << std::endl;
}

int InitMQTT()
{
    std::string path = CONFIG_PATH + "mqtt.configure";
    Json::Value root;
    std::fstream ifs(path);
    if(ifs.fail())
    {
        LOG_FATAL("MQTT configuration file failed to load,the file name is : " + path);
        return -1;
    }
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &root, &errs))
    {
        LOG_FATAL("MQTT Configuration file parsing failed.please check file format.");
        return -1;
    }

    Json::Value::Members members = root.getMemberNames();
    for (auto iter = members.begin(); iter != members.end(); iter++)
    {
        if(*iter != "clientId" &&
            *iter != "address" &&
            *iter != "username" &&
            *iter != "password" &&
            *iter != "aliveInterval" &&
            *iter != "cleanSession")
        {
            LOG_FATAL("MQTT Configuration missing required parameters.please check file format.");
            return -1;
        }
    }

    MQTTClientID = root["clientId"].asString();
    MQTTAddress = root["address"].asString();
    MQTTUsername = root["username"].asString();
    MQTTPasswd = root["password"].asString();
    MQTTiInterval = root["aliveInterval"].asInt();
    MQTTCleanSession = root["cleanSession"].asBool();
    return 0;
}

int InitIec61850()
{
    std::string path = CONFIG_PATH + "iec61850client.configure";
    Json::Value root;
    std::fstream ifs(path);
    if(ifs.fail())
    {
        LOG_FATAL("IEC61850 configuration file failed to load,the file name is : " + path);
        return -1;
    }
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &root, &errs))
    {
        LOG_FATAL("IEC61850 Configuration file parsing failed.please check file format.");
        return -1;
    }

    Json::Value::Members members = root.getMemberNames();
    for (auto iter = members.begin(); iter != members.end(); iter++)
    {
        if(*iter != "name" &&
            *iter != "remote_ip" &&
            *iter != "remote_port" &&
            *iter != "connect_timeout" &&
            *iter != "request_timeout" &&
            *iter != "reconnect_seconds")
        {
            LOG_FATAL("IEC61850 Configuration missing required parameters.please check file format.");
            return -1;
        }
    }

    IECName = root["name"].asString();
    IECIp = root["remote_ip"].asString();
    IECPort = root["remote_port"].asInt();
    IECConnectTimeout = root["connect_timeout"].asUInt();
    IECRequestTimeout = root["request_timeout"].asUInt();
    IECReconnectSeconds = root["reconnect_seconds"].asInt();
    return 0;
}