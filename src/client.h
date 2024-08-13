/*
 * @Author: your name
 * @Date: 2021-06-17 21:26:56
 * @LastEditTime: 2021-07-04 01:59:53
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /iec61850/src/client.h
 */
#ifndef __IEC_CLIENT_H__
#define __IEC_CLIENT_H__

#include <vector>
#include <atomic>
#include <functional>
#include <unordered_map>
#include "model.h"
#include "dataset.h"
#include "reports.h"
#include "sgcb.h"

using ConnectCallback = std::function<void(const int &)>;
using ReportCallback = std::function<void(const std::unordered_map<std::string ,std::string> &)>;

typedef struct
{
    std::string name_;
    std::string ip_;
    int port_;
    uint32_t connect_timeout_;      //单位：毫秒
    uint32_t request_timeout_;      //单位：毫秒
    int reconnect_seconds_;         //连接断开后重连的时间（秒）, 0:断开后立即重连, <0 不重连
} ConnectionParam;

class Iec61850Client
{
public:
    Iec61850Client(const ConnectionParam &param);
    ~Iec61850Client();
public:
    int connect(const bool &async = false);
    int reconnect();
    int disconnect();
    int init();
    int uninit();

    void loop();

    INode *getMode() const;

    // ReportControls
    int enbale();
    int disable();
    void report(const std::unordered_map<std::string ,std::string> &maps);

    // sgcbs
    int readSGCB(const std::string &ref,MmsValue *value);
    int writeSGCB(const std::string &ref,MmsValue *value,const uint8_t &num = -1);

    Datasets * getDatasets() const;
    Reports * getReports() const;

    ConnectionParam getConnectionParam() const;
private:
    int initModel();
    int initDatsets();
    int initReports();
    int initSgcbs();
private:
    ConnectionParam _param;         // 链接参数

    INode *_model;
    Datasets *_dataSets;
    Reports *_reports;
    Sgcbs *_sgcbs;

    ConnectCallback _connectCallback;
    ReportCallback _reportCallback;

    std::atomic_bool _enable;

    IedConnection _conn;
};

using Iec61850ClientPtr = std::shared_ptr<Iec61850Client>;

#endif // ! __IEC_CLIENT_H__