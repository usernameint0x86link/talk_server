#include "chatserver.h"
#include "chatservice.h"
#include "db.h"

#include <iostream>
#include <signal.h>

void reset_handler(int sig)
{
    ChatService::instance()->reset();
    exit(0);
}

int main(void)
{
    signal(SIGINT, reset_handler);

    muduo::net::EventLoop loop;
    muduo::net::InetAddress addr("127.0.0.1", 8800);
    std::string server_name("chat_server");
    ChatServer sever(&loop, addr, server_name);

    sever.start();
    loop.loop();

    return 0;
}