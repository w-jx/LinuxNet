// File Name: tcpclient.c
// Author: wenjx
// Created Time: 2020年12月03日 星期四 14时56分22秒

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#include <sys/socket.h>
#include <ctype.h>
#include <arpa/inet.h>
#define SERV_PORT 9999
//自己写的客户端程序
void geterror(char *s){
    perror(s);
    exit(-1) ;
}
int main(){

    int cfd=0;
    char buf[BUFSIZ];
    int ret=0;
    cfd = socket(AF_INET,SOCK_STREAM,0);
    if(cfd==-1)
        geterror("socker () error");
    struct sockaddr_in serv_addr;
    socklen_t serv_addr_len;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);

    int dst;
    inet_pton(AF_INET,"127.0.0.1",(void*)dst);
    serv_addr.sin_addr.s_addr=dst;

    serv_addr_len =sizeof(serv_addr);
    connect(cfd,(struct sockaddr*)&serv_addr,serv_addr_len);

    while(1)   {
        write(cfd,buf,sizeof(buf));

        ret = read(cfd,buf,ret);

        write(STDOUT_FILENO,buf,ret);

    }
    return 0 ;
}
