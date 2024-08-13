/*
 * @Author: your name
 * @Date: 2021-06-16 22:24:48
 * @LastEditTime: 2021-06-16 22:25:52
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /iec61850/src/mqtt/mqtt_client.h
 */
#ifndef __IEC_MQTT_CLIENT_H__
#define __IEC_MQTT_CLIENT_H__

#include <string>
#include <unordered_map>
#include <mqtt/async_client.h>

using namespace mqtt;
using namespace std;

using MessageCallback = std::function<void(const std::string &msg)>;

// 订阅回调
struct SubscribeListener : public virtual iaction_listener
{
    void on_failure(const token& tok) override;
    void on_success(const token& tok) override;
};

// 取消订阅回调
struct UnSubscribeListener : public virtual iaction_listener
{
    void on_failure(const token& tok) override;
    void on_success(const token& tok) override;
};

// 取消订阅回调
struct DeliveryActionListener : public virtual iaction_listener
{
    void setMessage(const std::string &msg);
    void on_failure(const token& tok) override;
    void on_success(const token& tok) override;
private:
    std::string _msg;
};

class MqttClient : public virtual async_client
{
public:
    explicit MqttClient(const string& serverURI, const string& clientId);
    ~MqttClient();
public:
    void registerMessageCallback(const std::string &topic,const MessageCallback &callback);

    token_ptr subscribe(const string& topicFilter, int qos);
    token_ptr unsubscribe(const string& topicFilter);
    delivery_token_ptr publish(string_ref topic, const void* payload, size_t n);
protected:
    void onConnected(const std::string &cause);
    void onDisConnected(const properties &prop, ReasonCode reasonCode);
    void onConnectLost(const std::string &cause);
    void onMessageArrived(const_message_ptr message);
private:

    std::unordered_map<std::string , MessageCallback> _mapMsgCallback;

    SubscribeListener _subscribeListener;
    UnSubscribeListener _unSubscribeListener;
    DeliveryActionListener _deliveryActionListener;
};

#endif // ! __IEC_MQTT_CLIENT_H__