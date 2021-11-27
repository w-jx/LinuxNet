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
#define SERV_PORT 9422
//自己写的客户端程序.一直有段错误，找不到原因
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

    //int dst;
    char * dst;
    ret = inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr.s_addr);
    if (ret==-1) {
        
        printf("inet_pton error :error is %s\n",strerror(errno));
        exit(1);
    }
    else if (ret==0) {
        printf("invalid address error :error is \n");
        exit(1);

    }
    //serv_addr.sin_addr.s_addr=dst;



    serv_addr_len =sizeof(serv_addr);
    connect(cfd,(struct sockaddr*)&serv_addr,serv_addr_len);

    while(1)   {
       ret = read(STDIN_FILENO,buf,sizeof(buf));
       write(cfd,buf,ret);

       ret = read(cfd,buf,ret);

       write(STDOUT_FILENO,buf,ret);

    }
    return 0 ;
}
