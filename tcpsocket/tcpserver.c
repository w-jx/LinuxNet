// File Name: tcpserver.c
// Author: wenjx
// Created Time: 2020年12月03日 星期四 14时04分42秒

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
//课程上的服务器设计，老师写的一模一样的，无法退出循环
void geterror(char *s){
    perror(s);
    exit(-1) ;
}
int main(){
    int lfd=0;
    int cfd=0;
    int ret=0;
    lfd = socket(AF_INET,SOCK_STREAM,0);
    if (lfd==-1) 
        geterror("socket() error");

    char buf[BUFSIZ];
    struct sockaddr_in serv_addr,clit_addr;
    socklen_t clit_addr_len;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    ret =  bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if(ret!=0) 
        geterror("bind error");
    listen(lfd,128);

    clit_addr_len = sizeof(clit_addr);
    cfd =accept(lfd,(struct sockaddr *)&clit_addr,&clit_addr_len);
    if (cfd==-1) 
        geterror("accept error");
    while(1) {
        ret = read(cfd,buf,sizeof(buf));
        write(STDOUT_FILENO,buf,ret);
        for(int i=0;i<ret;i++) {

            buf[i]=toupper(buf[i]);
        }
        write(cfd,buf,ret);
    }
    close(lfd);
    close(cfd);
    return 0 ;
}
