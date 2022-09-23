#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <semaphore.h>
#include <thread>
#include <atomic>
#include <string>
#include <unordered_map>
#include <chrono>

#include "json.hpp"
#include "public.h"
#include "user.h"
#include "group.h"

// 记录当前系统登录的用户信息
User g_current_user; // only read
// 记录当前登录用户的好友列表消息
std::vector<User> g_current_friends_list;
// 记录当前登录用户的群组列表消息
std::vector<Group> g_current_groups_list;
// 控制主菜单页面程序
bool g_is_main_menu_running = false;

// 用于读写线程之间的通信
sem_t rw_sem;

// 记录登录状态
std::atomic_bool g_is_login_success{false};

// 接收线程
void read_task_handler(int client_fd);

// 显示当前登录成功用户的基本信息
void show_current_user_data(void);

// 控制主菜单页面程序
void main_menu(int client_fd);

// 处理注册的响应逻辑
void do_login_response(nlohmann::json &response_js);

// 聊天客户端程序实现，main线程用作发送线程，子线程用作接收数据
int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "command invalid! example: ./ChatClient 127.0.0.1 8800" << std::endl;
        exit(-1);
    }

    const char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == client_fd)
    {
        std::cerr << "socket create error! " << std::endl;
        exit(-1);
    }

    sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    if (-1 == connect(client_fd, (struct sockaddr *)&server, sizeof(server)))
    {
        std::cerr << "connect to servererror! " << std::endl;
        close(client_fd);
        exit(-1);
    }

    while (true)
    {
        std::cout << "==============================" << std::endl;
        std::cout << "1. login" << std::endl;
        std::cout << "2. register" << std::endl;
        std::cout << "3. quit" << std::endl;
        std::cout << "==============================" << std::endl;
        std::cout << "choice:";
        int choice = 0;
        std::cin >> choice;
        std::cin.get(); // 读掉缓冲区残留的回车

        switch (choice)
        {
        case 1:
        {
            int id = 0;
            char pwd[50] = { 0 };
            std::cout << "userid: ";
            std::cin >> id;
            std::cin.get();
            std::cout << "password: ";
            std::cin.getline(pwd, 50);
            std::cin.get();

            nlohmann::json js;
            js["msgid"] = LOGIN_MSG;
            js["id"] = id;
            js["password"] = pwd;
            std::string request = js.dump();

            int len = send(client_fd, request.c_str(), request.size() + 1, 0);
            if (-1 == len)
            { std::cerr << "send login msg error:" << std::endl; }
            else
            {
                // 接收服务器反馈
                char buffer[1024] = { 0 };
                len = recv(client_fd, buffer, sizeof(buffer), 0);
                if(-1 == len)
                { std::cerr << "recv login msg error:" << std::endl; }
                else
                {
                    // 反序列化
                    nlohmann::json response_js = nlohmann::json::parse(buffer);
                    // 登录失败
                    if (response_js["errno"].get<int>() != 0)
                    { std::cerr << response_js["errnomsg"] << std::endl; }
                    else
                    {
                        // 登录成功，记录当前用户信息，好友信息，群组信息，离线消息
                        // 记录当前用户信息
                        g_current_user.set_id(response_js["id"].get<int>());
                        g_current_user.set_name(response_js["name"]);
                        // 好友信息
                        if (response_js.contains("friends"))
                        {
                            std::vector<std::string> vec = response_js["friends"];
                            for (std::string &str : vec)
                            {
                                nlohmann::json friend_js = nlohmann::json::parse(str);
                                User user;
                                user.set_id(friend_js["id"].get<int>());
                                user.set_name(friend_js["name"]);
                                user.set_state(friend_js["state"]);
                                g_current_friends_list.push_back(user);
                            }
                        }
                        // 群组信息
                        if (response_js.contains("groups"))
                        {
                            std::vector<std::string> vec = response_js["groups"];
                            for (std::string &str : vec)
                            {
                                nlohmann::json group_js = nlohmann::json::parse(str);
                                Group group;
                                group.set_id(group_js["id"].get<int>());
                                group.set_name(group_js["groupname"]);
                                group.set_desc(group_js["groupdesc"]);
                                g_current_groups_list.push_back(group);

                                std::vector<std::string> vec2 = group_js["users"];
                                for (std::string &str : vec2)
                                {
                                    GroupUser group_user;
                                    nlohmann::json group_js = nlohmann::json::parse(str);
                                    group_user.set_id(group_js["id"].get<int>());
                                    group_user.set_name(group_js["name"]);
                                    group_user.set_state(group_js["state"]);
                                    group_user.set_role(group_js["role"]);

                                    group.get_users().push_back(group_user);
                                }
                                g_current_groups_list.push_back(group);
                            }
                        }
                        
                        // 显示登录用户的基本信息
                        
                        show_current_user_data();

                        // 离线消息
                        if (response_js.contains("offlinemsg"))
                        {
                            std::vector<std::string> vec = response_js["offlinemsg"];
                            for (std::string &str : vec)
                            {
                                nlohmann::json js = nlohmann::json::parse(str);
                                std::cout << js["time"].get<std::string>() << "[" << js["id"] << "]" << js["name"].get<std::string>() << " said: " << js["msg"].get<std::string>() << std::endl;
                            }
                        }

                        // 登录成功
                        std::thread read_task(read_task_handler, client_fd);
                        read_task.detach();

                        g_is_main_menu_running = true;
                        main_menu(client_fd);
                    }
                }
            }

        }
        break;
        case 2: // 注册业务
        {
            char name[50] = { 0 };
            char pwd[50] = { 0 };
            std::cout << "username:";
            std::cin.getline(name, 50);
            std::cout << "userpasswd:";
            std::cin.getline(pwd, 50);

            nlohmann::json js;
            js["msgid"] = REG_MSG;
            js["name"] = name;
            js["password"] = pwd;
            std::string request = js.dump();

            int len = send(client_fd, request.c_str(), request.size() + 1, 0);
            if (-1 == len)
            { std::cerr << "send reg msg error:" << request << std::endl; }

            sem_wait(&rw_sem);
        }
        break;
        case 3: // 退出业务
        {
            close(client_fd);
            sem_destroy(&rw_sem);
            exit(0);
        }
        break;
        default:
            std::cerr << "invalid input!" << std::endl;
            break;
        }
    }
    
    return 0;
}

void show_current_user_data(void)
{
    std::cout << "------------------------login user------------------------" << std::endl;
    std::cout << "current login user ==> id: " << g_current_user.get_id() << " name: " << g_current_user.get_name() << std::endl;
    std::cout << "------------------------friend list------------------------" << std::endl;
    if (!g_current_friends_list.empty())
    {
        for (User &user : g_current_friends_list)
        { std::cout << user.get_id() << " " << user.get_name()  << " " << user.get_state() << std::endl; }
    }
    std::cout << "------------------------group list------------------------" << std::endl;
    if (!g_current_groups_list.empty())
    {
        for (Group &group : g_current_groups_list)
        { 
            std::cout << group.get_id() << " " << group.get_name() << " " << group.get_desc() << std::endl; 
        
            std::cout << "================group user================" << std::endl;
            for (GroupUser &group_user : group.get_users())
            {
                std::cout << group_user.get_id() << " " << group_user.get_name() << " " << group_user.get_role() << " " << group_user.get_state() << std::endl;
            }
        }
    }
    std::cout << "---------------------------------------------------------" << std::endl;
}

std::unordered_map<std::string, std::string> command_map = {
    {"help", "显示所有支持命令,格式help"},
    {"chat", "一对一聊天,格式chat:friend:message"},
    {"addfriend", "添加好友,格式addfriend:friendid"},
    {"creategroup", "创建群组,格式creategroup:groupname:groupdesc"},
    {"addgroup", "加入群组,格式addgroup:groupid"},
    {"groupchat", "群聊,格式groupchat:groupid:message"},
    {"loginout", "注销,格式loginout"}
};

void help(int client_fd = -1, std::string message = "")
{
    std::cout << "show command list========================================" << std::endl;
    for (auto &p : command_map)
    { std::cout << p.first << ":" << p.second << std::endl; }
    std::cout << "=========================================================" << std::endl;
    std::cout << std::endl;
}

void chat(int, std::string);
void loginout(int, std::string);
void addgroup(int, std::string);
void creategroup(int, std::string);
void groupchat(int, std::string);
void addfriend(int, std::string);
void creategroup(int, std::string);


std::unordered_map<std::string, std::function<void(int, std::string)>> command_handler_map = {
    {"help", help},
    {"chat", chat},
    {"loginout", loginout},
    {"addgroup", addgroup},
    {"groupchat", groupchat},
    {"addfriend", addfriend},
    {"creategroup", creategroup}
};

std::string get_current_time(void)
{
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&time);
    char date[60]{0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday, (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}

void addgroup(int client_fd, std::string str)
{
    int group_id = atoi(str.c_str());

    nlohmann::json js;
    js["msgid"] = ADD_GROUP_MSG;
    js["id"] = g_current_user.get_id();
    js["groupid"] = group_id;

    std::string request = js.dump();

    int len = send(client_fd, request.c_str(), request.size() + 1, 0);
    if (-1 == len)
    { std::cerr << "send addgroup msg error" << std::endl; }
}

void groupchat(int client_fd, std::string str)
{
    int index = str.find(":");
    if (-1 == index)
    {
        std::cerr << "groupchat msg error" << std::endl;
        return ;
    }

    int group_id = atoi(str.substr(0, index).c_str());
    std::string message = str.substr(index + 1, str.size() -index);

    nlohmann::json js;
    js["msgid"] = GROUP_CHAT_MSG;
    js["id"] = g_current_user.get_id();
    js["name"] = g_current_user.get_name();
    js["groupid"] = group_id;
    js["msg"] = message;
    js["time"] = get_current_time();

    std::string request = js.dump();

    int len = send(client_fd, request.c_str(), request.size() + 1, 0);
    if (-1 == len)
    { std::cerr << "send groupchat msg error" << std::endl; }
}

void creategroup(int client_fd, std::string str)
{
    int index = str.find(":");
    if (-1 == index)
    {
        std::cerr << "creategroup cmmand invalid" << std::endl;
        return;
    }

    std::string group_name = str.substr(0, index);
    std::string group_desc = str.substr(index + 1, str.size() - index);

    nlohmann::json js;
    js["msgid"] = CREATE_GROUP_MSG;
    js["id"] = g_current_user.get_id();
    js["groupname"] = group_name;
    js["groupdesc"] = group_desc;

    std::string request = js.dump();

    int len = send(client_fd, request.c_str(), request.size() + 1, 0);
    if (-1 == len)
    { std::cerr << "send creategroup msg error" << std::endl; }
}

void loginout(int client_fd, std::string str)
{
    nlohmann::json js;
    js["msgid"] = LOGINOUT_MSG;
    js["id"] = g_current_user.get_id();
    std::string buffer = js.dump();

    std::string request = js.dump();

    int len = send(client_fd, request.c_str(), request.size() + 1, 0);
    if (-1 == len)
    { std::cerr << "send groupchat msg error" << std::endl; }
    else
    {
        g_is_main_menu_running = false;
        g_current_friends_list.clear();
        g_current_groups_list.clear();
    }
}

void addfriend(int client_fd, std::string str)
{
    int friend_id = atoi(str.c_str());
    
    nlohmann::json js;
    js["msgid"] = ADD_FRIEND_MSG;
    js["id"] = g_current_user.get_id();
    js["friend_id"] = friend_id;

    std::string reques = js.dump();

    int len = send(client_fd, reques.c_str(), reques.size() + 1, 0);
    if (-1 == len)
    { std::cerr << "send addfriend msg error" << std::endl; }
}

void chat(int client_fd, std::string str)
{
    int index = str.find(":");
    if (-1 == index)
    { std::cerr << "chat command invalid" << std::endl; }

    int friend_id = atoi(str.substr(0, index).c_str());
    std::string message = str.substr(index + 1, str.size() - index);

    nlohmann::json js;
    js["msgid"] = ONE_CHAT_MSG;
    js["id"] = g_current_user.get_id();
    js["name"] = g_current_user.get_name();
    js["to"] = friend_id;
    js["msg"] = message;
    js["time"] = get_current_time();

    std::string request = js.dump();

    int len = send(client_fd, request.c_str(), request.size() + 1, 0);
    if (-1 == len)
    { std::cerr << "send chat msg error" << std::endl; }
}

void main_menu(int client_fd)
{
    help();

    char buffer[1024] = { 0 };
    while (g_is_main_menu_running)
    {
        std::cin.getline(buffer, 1024);
        std::string command_buffer(buffer);
        // 存储命令
        std::string command;
        int index = command_buffer.find(":");
        if (-1 == index) // help or loginout
        { command = command_buffer; }
        else
        { command = command_buffer.substr(0, index); }

        auto it = command_handler_map.find(command);
        if (it == command_handler_map.end())
        {
            std::cerr << "invalie input command!" << std::endl;
            continue;
        }

        it->second(client_fd, command_buffer.substr(index+1,command_buffer.size()-index));
    }
}

void do_login_response(nlohmann::json &response_js)
{
    if (0 != response_js["errno"].get<int>()) // 登录失败
    {
        std::cerr << response_js["errno"] << std::endl;
        g_is_login_success = false;
    }
    else // 登录成功
    {
        g_current_user.set_id((response_js["id"].get<int>()));
        g_current_user.set_name(response_js["name"]);
        
        // 记录当前用户的好友列表信息
        if (response_js.contains("friends"))
        {
            // TO_DO
        }
        // 记录当前用户的群组列表信息
        if (response_js.contains("groups"))
        {
            // TO_DO
        }
        // TO_DO

        g_is_login_success = true;
    }
}

void read_task_handler(int client_fd)
{
    while (true)
    {
        char buffer[1024] = {0};
        int len = recv(client_fd, buffer, 1024, 0);
        if (-1 == len || 0 == len)
        {
            close(client_fd);
            exit(-1);
        }

        nlohmann::json js = nlohmann::json::parse(buffer);
        int msg_type = js["msgid"].get<int>();
        // 一对一聊天
        if (ONE_CHAT_MSG == msg_type)
        {
            std::cout << js["time"].get<std::string>() << "[" << js["id"] << "]" << js["name"].get<std::string>() << " said: " << js["msg"].get<std::string>() << std::endl;
            continue;
        }
        if (LOGIN_MSG_ACK == msg_type)
        {
            do_login_response(js);
            sem_post(&rw_sem);
            continue;
        }
        // TO_DO
    }
}























