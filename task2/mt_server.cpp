#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <atomic>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
char *server_ip = "82.156.14.223";
char buff[BUF_SIZE];                              //数据缓冲区
std::vector<std::pair<int, std::string>> clients; //保存连接到服务器的用户
std::mutex m_mutex;
void error_handing(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
//向加入聊天室的成员发送消息
void broad_cast(const char *message, int message_len)
{
    for (auto client : clients)
    {
        write(client.first, message, message_len);
    }
}
//线程主函数
void worker_thread(int clnt_sock)
{

    int str_len = 0, i;
    char msg[BUF_SIZE];
    //用户输入用户名并保存
    char hello_msg[] = {"欢迎加入聊天室，请输入用户名："};
    write(clnt_sock, hello_msg, sizeof(hello_msg));
    if(read(clnt_sock, msg, sizeof(msg))<0)
        error_handing("user name error");

    std::string user_name = msg;
    clients.push_back(std::pair<int, std::string>(clnt_sock, user_name));

    std::string chat_home_hello_msg = user_name + "加入房间，当前用户数：" + std::to_string(clients.size());
    std::cout<<chat_home_hello_msg<<std::endl;
    const char *c_hello_mag = chat_home_hello_msg.c_str();

    m_mutex.lock();
    broad_cast(c_hello_mag, chat_home_hello_msg.size());
    m_mutex.unlock();

    //用户聊天信息处理
    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
    { //用户发过来的不是EOF
        m_mutex.lock();
        broad_cast(msg, str_len);
        m_mutex.unlock();
    }
    //用户退出房间
    { //当收到EOF
        m_mutex.lock();
        auto p = std::find(clients.begin(), clients.end(), std::pair<int, std::string>(clnt_sock, user_name));
        clients.erase(p);
        std::string bye_msg = "用户";
        bye_msg = bye_msg + user_name + "退出了房间，当前用户数:" + std::to_string(clients.size());
        const char *c_bye_msg = bye_msg.c_str();
        broad_cast(c_bye_msg, bye_msg.size());
        m_mutex.unlock();
        std::cout<<bye_msg<<std::endl;
    }
    close(clnt_sock);
}
int main(int argc, char *argv[])
{

    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t serv_addr_size, clnt_addr_size;
    char read_buf[BUF_SIZE], write_buf[BUF_SIZE];
    int str_len;


    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
    {
        error_handing("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server_ip);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        error_handing("bind() error");
    }
    if (listen(serv_sock, 5) == -1)
    {
        error_handing("listen() error");
    }

    while (1)
    {
        clnt_addr_size = sizeof(clnt_sock);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
        {
            error_handing("accept() error");
        }
        std::thread clnt_server(worker_thread, clnt_sock);
        
        clnt_server.detach();
    }

    close(serv_sock);
    return 0;
}