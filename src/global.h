/*
 * @Author: your name
 * @Date: 2021-06-16 22:53:22
 * @LastEditTime: 2021-07-04 19:39:06
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /iec61850/src/global.h
 */
#ifndef __IEC_GLOBAL_H__
#define __IEC_GLOBAL_H__

#include "mqtt/mqtt_client.h"
#include "gwmanager/gwdata.h"

// MQTT 
extern std::string MQTTClientID;
extern std::string MQTTAddress;
extern std::string MQTTUsername;
extern std::string MQTTPasswd;
extern int MQTTiInterval;
extern bool MQTTCleanSession;

extern MqttClient *AsyncClient;

//GWManager 数据对接
extern GWData *GwData;

// 61850
// 后期需要实现管理类,替代给
extern std::string IECName;
extern std::string IECIp;
extern int IECPort;
extern uint32_t IECConnectTimeout;
extern uint32_t IECRequestTimeout;
extern int IECReconnectSeconds;

#endif // ! __IEC_GLOBAL_H__