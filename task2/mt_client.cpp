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
std::mutex m_mutex;
char *server_ip = "82.156.14.223";
char *server_port="33000";

std::string user_name;
int sock;
void error_handing(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
//发送消息的线程
void send_msg()
{
    std::string message;
    while (1)
    {
        getline(std::cin, message);
        message = user_name + '\t' + message;

        write(sock, message.c_str(), message.size());
    }
}
//收到消息的线程
void recv_msg()
{
    char recv_buf[BUF_SIZE];
    int recv_len = 0;
    while (1)
    {
        recv_len = read(sock, recv_buf, BUF_SIZE - 1);

        recv_buf[recv_len] = 0;
        puts(recv_buf);
    }
}
int main(int argc, char *argv[])
{

    struct sockaddr_in serv_addr;
    char buf[BUF_SIZE];
   
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        error_handing("socket() error");
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
 
    serv_addr.sin_addr.s_addr = inet_addr(server_ip);
    serv_addr.sin_port = htons(atoi(server_port));
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        error_handing("connect() error");
    }
    //读取hello_message,并向服务器发送用户名
    int hello_str_len;
    hello_str_len = read(sock, buf, BUF_SIZE);
    buf[hello_str_len] = 0;
    fputs(buf, stdout);
    getline(std::cin, user_name);
    user_name+='\0';

    m_mutex.lock();
    write(sock, user_name.c_str(), user_name.size());
    
    m_mutex.unlock();
    user_name.pop_back();
    user_name.insert(user_name.begin(), '[');
    user_name += "]";


    std::thread send_thread(send_msg);
    std::thread recv_thread(recv_msg);
    send_thread.detach();
    recv_thread.detach();
    //防止主线程提前终止
    while (1)
        ;
}