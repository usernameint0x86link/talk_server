#include "chatservice.h"
#include "public.h"

#include <vector>
#include <functional>
#include <muduo/base/Logging.h>

using namespace std;
using namespace placeholders;

ChatService* ChatService::instance(void)
{
    static ChatService service;
    return &service;
}

void ChatService::reset(void)
{
    m_user_model.reset_state();
}

// 注册消息以及对应的Handler回调操作
ChatService::ChatService(void)
{
    // 用户基本业务管理相关事件处理回调注册
    m_msg_handler_map.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    m_msg_handler_map.insert({REG_MSG, std::bind(&ChatService::reg, this, \
    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    m_msg_handler_map.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, \
    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
}

// 处理登录业务 id  pwd
void ChatService::login(const muduo::net::TcpConnectionPtr &conn, \
    nlohmann::json &js, muduo::Timestamp time)
{
    // LOG_INFO << "do login service!";
    int id = js["id"].get<int>();
    std::string pwd = js["password"];

    User user = m_user_model.query(id);
    if (user.get_id() == id && user.get_password() == pwd)
    {
        if (user.get_state() == "online") // 用于已经登录，不允许重复登录
        {
            nlohmann::json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "the account is already logged in";
            conn->send(response.dump());
        }
        {
            std::lock_guard<std::mutex> lock(m_conn_mutex);
            m_user_conn_map.insert({id, conn});
        }

        // 登录成功，更新用户状态信息 state offline --> online
        user.set_state("online");
        m_user_model.update_state(user);

        nlohmann::json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.get_id();
        response["name"] = user.get_name();

        // 查询该用户是否有离线消息
        std::vector<std::string> vec = m_offline_msg_model.query(id);
        if (vec.size() > 0)
        { 
            response["offlinemsg"] = vec; 
            m_offline_msg_model.remove(id);
        }
        conn->send(response.dump());
    }
    else // 该用户不存在OR用户存在但密码错误，登录失败
    {
        nlohmann::json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "usrname or password error";
        conn->send(response.dump());
    }
}
// 处理注册业务 name pwd
void ChatService::reg(const muduo::net::TcpConnectionPtr &conn, \
    nlohmann::json &js, muduo::Timestamp time)
{
    // LOG_INFO << "do reg service!";
    std::string user_name = js["name"];
    std::string user_password = js["password"];

    User user;
    user.set_name(user_name);
    user.set_password(user_password);
    bool state = m_user_model.insert(user);
    if (state) // 注册成功
    {
        nlohmann::json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.get_id();
        conn->send(response.dump());
    }
    else // 注册失败
    {
        nlohmann::json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}
// 获取单例对象的接口函数
MsgHandler ChatService::get_handler(int msgid)
{
    auto it = m_msg_handler_map.find(msgid);
    if (m_msg_handler_map.end() == it)
    {
        LOG_ERROR << "msgid" << msgid << " can not find handler";
    }

    return m_msg_handler_map[msgid];
}


void ChatService::client_close_exception(const muduo::net::TcpConnectionPtr &conn)
{
    User user;
    {
        std::lock_guard<std::mutex> lock(m_conn_mutex);
        for (auto it = m_user_conn_map.begin(); it != m_user_conn_map.end(); ++it)
        {
            if (it->second == conn)
            {
                user.set_id(it->first);
                m_user_conn_map.erase(it);
                break;
            }
        }
    }
    
    if (user.get_id() != -1)
    {
        user.set_state("offline");
        m_user_model.update_state(user);
    }
}

void ChatService::oneChat(const muduo::net::TcpConnectionPtr &conn, \
    nlohmann::json &js, muduo::Timestamp timestamp)
{
    int toid = js["to"].get<int>();

    {
        std::lock_guard<std::mutex> lock(m_conn_mutex);
        auto it = m_user_conn_map.find(toid);
        if (it != m_user_conn_map.end())
        {
            it->second->send(js.dump());
            return;
        }
    }

    // toid不在线，存储离线消息
    m_offline_msg_model.insert(toid, js.dump());
}