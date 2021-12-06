#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
void error_handing(char *message);
char *port="9101";
int main(int argc, char *argv[])
{
    int serv_sock;
    int clnt_sork;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    char message[BUF_SIZE];
    int str_len;
    //创建一个套接字，参数分别为协议族信息，套接字类型，以及最终决定选择什么协议的第三个参数
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); //创建一个socket
    if (serv_sock == -1)
    {
        error_handing("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr)); //把地址全部置为0
    //设置服务器地址属性
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(port));
    //将套接字绑定到端口之上(为套接字分配ip和端口号)
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handing("bind() error");
    //监听套接字，生成服务器端套接字（用于接受连接请求） ，将客户端发来的连接请求放入等待队列
    if (listen(serv_sock, 5) == -1)
        error_handing("listen() error");
    while(1)
    {
        clnt_addr_size = sizeof(clnt_addr);
        //当客户端发来请求时接受，自动生成一个套接字（用于数据I/O），并自动与客户端建立连接,返回与客户端建立连接的套接字
        clnt_sork = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sork == -1)
        {
            error_handing("accept() error");
        }
        else{
            printf("成功连接到客户端\n");
        }
        while((str_len=read(clnt_sork,message,BUF_SIZE))!=0)
        {
            printf("从客户端接受到的消息:%s",message);
            write(clnt_sork,message,str_len);
        }
        
        close(clnt_sork);
    }
    close(serv_sock);
    return 0;
}

void error_handing(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}