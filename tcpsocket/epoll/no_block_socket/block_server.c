#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <strings.h>
#include <poll.h>
#include <sys/epoll.h>
#define SERV_PORT 9999
#define MAXLINE 10
//和pipe_epoll.c完全一样，只不过变成了套接字，阻塞模式下的
int main()
{

    int lfd = 0;
    int cfd = 0;
    int sockfd;

    int i = 0;
    int ret = 0;
    int n;

    char buf[BUFSIZ], ip[32];
    int efd; //epoll_create返回值

    lfd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1; //设置端口复用
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));

    struct sockaddr_in serv_addr, clit_addr;
    bzero(&serv_addr, sizeof(serv_addr)); //将结构体清零
    socklen_t clit_addr_len;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //自动匹配有效IP
    Bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    Listen(lfd, 128);

    struct pollfd client[1024];
    struct epoll_event event;
    struct epoll_event resevent[10];
    efd = epoll_create(10);
    int res, len, flag;

    event.events = EPOLLIN | EPOLLET; //边沿触发，默认水平触发
    //event.events=EPOLLIN;
    printf("waiting for connections\n");
    clit_addr_len = sizeof(clit_addr);
    cfd = Accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);
    printf("clinet from %s and port:%d\n", inet_ntop(AF_INET, &clit_addr.sin_addr, ip, sizeof(ip)), ntohs(clit_addr.sin_port));

    //flag = fcntl(cfd, F_GETFL); //先获取位图设置
    //flag |= O_NONBLOCK;         //添加非阻塞属性
    //fcntl(cfd, F_SETFL, flag);  //设置属性

    event.data.fd = cfd;
    epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &event); //将cfd添加到监听红黑树
    printf("epoll wait begin\n");
    while (1)
    {
        res = epoll_wait(efd, resevent, 10, -1); //阻塞监听
        printf("epoll wait end res %d\n", res);

        if (resevent[0].data.fd == cfd)
        {
            len = read(cfd, buf, MAXLINE / 2);

            write(STDOUT_FILENO, buf, len);

        } //fd ==cfd

    } //while(1)
}
