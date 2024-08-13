/*
 * @Author: your name
 * @Date: 2021-06-16 22:30:38
 * @LastEditTime: 2021-07-03 18:31:15
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /iec61850/src/main.cpp
 */


#include "client.h"
#include "global.h"
#include "init.h"
#include "internal/log.h"

int main(int argc, char *argv[])
{
    if(ShowInfo(argc,argv) == -1)
    {
        return -1;
    }
    if(Init() != 0)
    {
        LOG_FATAL("MQTT or IEC61850 configure module init failed. applicaton exit.");
        return -1;
    }
    else
    {
        LOG_INFO("The application is initialized successfully, ready to start...");
    }
    LOG_INFO("MQTT connection parameters : {address = " + MQTTAddress +
            ",ClientId = " + MQTTClientID +
            ",user = " + MQTTUsername +
            ",passwd = ******,cleanSession = " + (MQTTCleanSession ? "true" : "false") +
            ",interval = " + std::to_string(MQTTiInterval) + "}");
    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(MQTTiInterval);
    connOpts.set_clean_session(MQTTCleanSession);
    connOpts.set_user_name(MQTTUsername);
    connOpts.set_password(MQTTPasswd);
    if(!AsyncClient)
    {
        AsyncClient = new MqttClient(MQTTAddress, MQTTClientID);
    }
    auto token = AsyncClient->connect(connOpts);
    token->wait();
    LOG_INFO("MQTT connect successfully.");

    GwData = new GWData();
    GwData->start();
    GwData->wait();
    LOG_INFO("GWManager init successfully.");

    LOG_INFO("IEC61850 connection parameters : {name = " + IECName +
            ",ip = " + IECIp +
            ",port = " + std::to_string(IECPort) +
            ",ConnectTimeout = " + std::to_string(IECConnectTimeout) +
            ",RequestTimeout = " + std::to_string(IECRequestTimeout) +
            ",ReconnectSeconds = " + std::to_string(IECReconnectSeconds) + "}");

    ConnectionParam param
    {
        IECName,
        IECIp,
        IECPort,
        IECConnectTimeout,
        IECRequestTimeout,
        IECReconnectSeconds,
    };
    
    auto iecClient = std::make_shared<Iec61850Client>(param);

    if(iecClient->connect() == 0)
    {
        if(iecClient->init() != 0)
        {
            LOG_FATAL("Init iec client failed, Application exit...");
            return -1;
        }
    }

    iecClient->loop();

    // if(GwData)
    // {
    //     delete GwData;
    //     GwData = nullptr;
    // }

    if(AsyncClient)
    {
        delete AsyncClient;
        AsyncClient = nullptr;
    }

    LOG_INFO("Application end ...");
    return 0;
}
