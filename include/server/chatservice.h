#ifndef __CHATSERVICE_H__
#define __CHATSERVICE_H__

#include <muduo/net/TcpConnection.h>

#include <functional>
#include <mutex>
#include <unordered_map>

#include "json.hpp"
#include "offlinemessagemodel.h"
#include "usermodel.h"
#include "friendmodel.h"

using MsgHandler = std::function<void(const muduo::net::TcpConnectionPtr &,
                                      nlohmann::json &, muduo::Timestamp time)>;

// 聊天服务器业务类
class ChatService {
 public:
  // 获取单例对象的接口函数
  static ChatService *instance(void);
  // 处理登录业务
  void login(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js,
             muduo::Timestamp timestamp);
  // 处理注册业务
  void reg(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js,
           muduo::Timestamp timestamp);
  // 一对一聊天业务
  void one_chat(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js,
                muduo::Timestamp timestamp);
  // 添加好友服务
  void add_friend(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js,
                  muduo::Timestamp timestamp);
  // 添加群组服务
  void create_group(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp);
  // 加入群组服务
  void add_group(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp);
  // 群组聊天服务
  void group_chat(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp);

  void login_out(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp);

  // 处理客户端异常退出
  void client_close_exception(const muduo::net::TcpConnectionPtr &conn);
  // 服务器异常退出，业务重置方法
  void reset(void);
  // 获取消息对应的处理器
  MsgHandler get_handler(int msgid);

 private:
  ChatService(void);
  // 存放消息id和其对应的业务处理函数
  std::unordered_map<int, MsgHandler> m_msg_handler_map;
  // 存储在线用户的通信连接
  std::unordered_map<int, muduo::net::TcpConnectionPtr> m_user_conn_map;
  std::mutex m_conn_mutex;

  UserModel m_user_model;
  OfflineMsgModel m_offline_msg_model;
  FriendModel m_friend_model;
};

#endif