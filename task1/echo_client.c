#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
char *ip="127.0.0.1";
char *port="9101";
void error_handing(char *message);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char send_message[BUF_SIZE],recv_message[BUF_SIZE];
    int str_len;


    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        error_handing("sock() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(atoi(port));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handing("connect() error");
    else
        fputs("成功连接到服务器\n", stdout);
    //send_len表示当前已发送的总长度，
    int send_len,recv_len,recv_cnt;//recv_len表示当前从服务器接收到的数据报的总长度，而recv_cnt是本次循环从服务器接收到的数据报的长度
    while (1)
    {
        send_len=0;
        printf("向服务器发送消息:");
        send_len=strlen(fgets(send_message, BUF_SIZE, stdin)); //向服务器发送字符串,把已发送长度存入send_len
        write(sock, send_message, strlen(send_message));
        
        
        recv_len=0;
        printf("\n从服务器接受到的消息:\n");
        while(recv_len<send_len){
            recv_cnt=read(sock,recv_message,BUF_SIZE-1);
            recv_message[recv_cnt]=0;
            recv_len+=recv_cnt;
            puts(recv_message);
        }
        
        
    }

    close(sock);
    return 0;
}

void error_handing(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}