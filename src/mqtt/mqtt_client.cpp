#include "mqtt_client.h"
#include "../internal/log.h"

void SubscribeListener::on_failure(const token& tok)
{
    if (tok.get_message_id() != 0)
    {
        LOG_ERROR("Subscription failed,token is : [ " + std::to_string(tok.get_message_id()) + " ]");
    }
    else 
        LOG_WARN("Subscription abnormal.");
}

void SubscribeListener::on_success(const token& tok)
{
    if (tok.get_message_id() != 0)
    {
        auto top = tok.get_topics();
        if (top && !top->empty())
        {
            LOG_INFO("Subscription successfully,token is : [ " + 
            std::to_string(tok.get_message_id()) + " ],topic is : [ " + (*top)[0] + "]");
        }
    }
    else 
        LOG_WARN("Subscription abnormal.");
}

void UnSubscribeListener::on_failure(const token& tok)
{
    if (tok.get_message_id() != 0)
    {
        LOG_ERROR("UnSubscription failed,token is : [ " + std::to_string(tok.get_message_id()) + " ]");
    }
    else 
        LOG_WARN("UnSubscription abnormal.");
}

void UnSubscribeListener::on_success(const token& tok)
{
    if (tok.get_message_id() != 0)
    {
        auto top = tok.get_topics();
        if (top && !top->empty())
        {
            LOG_INFO("UnSubscription successfully,token is : [ " + 
            std::to_string(tok.get_message_id()) + " ],topic is : [ " + (*top)[0] + "]");
        }
    }
    else 
        LOG_WARN("UnSubscription abnormal.");
}

void DeliveryActionListener::setMessage(const std::string &msg)
{
    if(_msg != msg) {
        _msg = msg;
    }
}

void DeliveryActionListener::on_failure(const token& tok)
{
    if (tok.get_message_id() != 0)
    {
        LOG_ERROR("Send mqtt message failed,token is : [ " + std::to_string(tok.get_message_id()) + " ]" +
            ",msg is : [ " + _msg + " ]");
    }
    else 
        LOG_WARN("Message sending abnormal.");
}

void DeliveryActionListener::on_success(const token& tok)
{
    if (tok.get_message_id() != 0)
    {
        auto top = tok.get_topics();
        if (top && !top->empty())
        {
            LOG_INFO("Send mqtt message successfully,token is : [ " + std::to_string(tok.get_message_id()) + " ]" +
            ",topic is : [" + (*top)[0] +" ],msg is : [ " + _msg + " ]");
        }
    }
    else 
        LOG_WARN("Message sending abnormal.");
}


MqttClient::MqttClient(const string& serverURI, const string& clientId)
    :async_client(serverURI,clientId)
{
    using namespace std::placeholders;
    set_connected_handler(std::bind(&MqttClient::onConnected,this,_1));
    set_disconnected_handler(std::bind(&MqttClient::onDisConnected,this,_1,_2));
    set_connection_lost_handler(std::bind(&MqttClient::onConnectLost,this,_1));
    set_message_callback(std::bind(&MqttClient::onMessageArrived,this,_1));
}

MqttClient::~MqttClient()
{
}

void MqttClient::registerMessageCallback(const std::string &topic,const MessageCallback &callback)
{
    LOG_DEBUG("Register MQTT topic callback function.");
    if(topic.empty() || !callback)
    {
        LOG_ERROR("Invalid topic name or callback function.");
        return;
    }
    _mapMsgCallback.insert(std::make_pair(topic,callback));
}

token_ptr MqttClient::subscribe(const string& topicFilter, int qos)
{
    return async_client::subscribe(topicFilter,qos,nullptr,_subscribeListener);
}

token_ptr MqttClient::unsubscribe(const string& topicFilter)
{
    return async_client::unsubscribe(topicFilter,nullptr,_unSubscribeListener);
}

delivery_token_ptr MqttClient::publish(string_ref topic, const void* payload, size_t n)
{
    _deliveryActionListener.setMessage(std::string((char *)payload,n));
    return async_client::publish(topic,payload,n,
            message::DFLT_QOS + 1,message::DFLT_RETAINED,nullptr,_deliveryActionListener);
}

void MqttClient::onConnected(const std::string &cause)
{
    LOG_DEBUG("MQTT connected sucess.");
}

void MqttClient::onDisConnected(const properties &prop, ReasonCode reasonCode)
{
    LOG_DEBUG("MQTT disconnected sucess.");
}

void MqttClient::onConnectLost(const std::string &cause)
{
    LOG_WARN("MQTT connection lost.");
}

void MqttClient::onMessageArrived(const_message_ptr message)
{
    LOG_INFO("Received New message,topic is : [ " + message->get_topic() + 
        "],qos is : [ " + std::to_string(message->get_qos()) + 
        "],payload is : [ " + message->get_payload() + 
        "],msg is :[ " + message->to_string() + " ]");
    // 通过topic找到对应的回调函数
    auto it = _mapMsgCallback.begin();
    for (; it != _mapMsgCallback.end(); it++)
    {
        if(message->get_topic() == it->first)
        {
            break;
        }
    }
    if(it != _mapMsgCallback.end())
    {
        (it->second)(message->to_string());
    }
    else
    {
        LOG_WARN("The function callback for the current topic was not found,please check register function.");
    }
}


MqttClient *AsyncClient = nullptr;