//#include <./wrap.h>
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
//客户端地址读取
int main(){
    int lfd=0;
    int cfd=0;
    int ret=0;
    lfd = Socket(AF_INET,SOCK_STREAM,0);
    if (lfd==-1) 
        geterror("socket() error");

    char buf[BUFSIZ],cliet_ip[1024];
    struct sockaddr_in serv_addr,clit_addr;
    socklen_t clit_addr_len;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    ret =  Bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    Listen(lfd,128);

    clit_addr_len = sizeof(clit_addr);
    cfd =Accept(lfd,(struct sockaddr *)&clit_addr,&clit_addr_len);

    printf("client ip=%s,port=%d\n",
            inet_ntop(AF_INET,&clit_addr.sin_addr.s_addr,cliet_ip,sizeof(cliet_ip)),
            ntohs(clit_addr.sin_port));
    printf("char client ip=%s\n",cliet_ip);

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
