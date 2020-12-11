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

void geterror(char *s){
    perror(s);
    exit(-1) ;
}



int Socket(int domain, int type, int protocol){

    int fd;

    fd =socket(domain,type,protocol);

    if(fd ==-1){
        geterror("socket error");
        return -1;
    }
    return fd;
}
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {

    int ret;

    ret = bind(sockfd,addr,addrlen);
    if (ret ==-1) {
        geterror("bind error");
        return -1;
    }
    return 0;
}
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {

    int ret = 0;
    ret = connect(sockfd,addr,addrlen);
    if(ret ==-1) {

        geterror("connect error");
        return -1;
    }
    return 0;
}
int Listen(int sockfd, int backlog) {

    int ret = 0;
    ret = listen(sockfd,backlog);
    if (ret ==-1) {

        geterror("listen error");
        return -1;

    }
    return 0;

}
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {

int retfd;

retfd =accept(sockfd,addr,addrlen);
if (retfd ==-1) {

geterror("accept error");
return -1;
}
return retfd;

}



