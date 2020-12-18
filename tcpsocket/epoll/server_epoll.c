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
#define SERV_PORT 9998
//epoll方式实现的
int main()
{
    int lfd = 0;
    int cfd = 0;
    int sockfd;
    int maxfd = 0; //最大的文件描述符
    int i = 0;
    int ret = 0;
    int n;
    int maxi = 0; //数组有效下标
    char buf[BUFSIZ];
    int efd; //epoll_create返回值

    lfd = Socket(AF_INET, SOCK_STREAM, 0);
    struct pollfd client[1024];
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
    struct epoll_event tep, ep[1024]; //tep:epoll_ctl参数，ep[]：epoll_wait参数
    efd = epoll_create(1024);         //创建epoll模型，efd指向红黑树根节点
    if (efd == -1)
        geterror("epoll create error");

    tep.data.fd = lfd;
    tep.events = EPOLLIN; //读事件
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, lfd, &tep);
    if (efd == -1)
        geterror("epoll ctl error");

    while (1)
    {
        ret = epoll_wait(efd, ep, 1024, -1); //阻塞监听是否有客户端连接请求,-1表示阻塞等待
        if (ret < 0)
            geterror("epoll wait error");
        if (ret > 0)
        { //说明有客户端连接
            for (i = 0; i < ret; i++)
            
         {
             if (ep[i].events & EPOLLIN)
             { //lfd满足监听的读事件
             if (ep[i].data.fd==lfd) //判断是不是lfd
                 clit_addr_len = sizeof(clit_addr);
                 cfd = Accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);
                 //注意这个accept()不会阻塞
                 for (i = 1; i < FD_SETSIZE; i++)
                 { //找到client[]空闲的位置
                     if (client[i].fd < 0)
                     {
                         client[i].fd = cfd;
                         break;
                     }
                 }
                 if (i == FD_SETSIZE)
                 {
                     fputs("too many clients\n", stderr);
                     exit(1);
                 }
                 client[i].events = POLLIN;
                 if (i > maxi)
                     maxi = i; //保证maxi存储的总是client[]最大的下标
                 if (ret == 1)
                     continue;
             } //client[0].revents &POLLIN
            for (i = 1; i <= maxi; i++)
            {
                if ((sockfd = client[i].fd) < 0)
                    continue;
                if (client[i].revents & POLLIN)
                { //找到满足读事件的fd
                    if ((n = read(sockfd, buf, sizeof(buf))) == 0)
                    { //客户端关闭连接了，服务器端也应该关闭对应连接
                        close(sockfd);
                        client[i].fd = -1;
                    }
                    else if (n > 0)
                    {
                        for (int j = 0; j < n; j++)
                            buf[j] = toupper(buf[j]);
                        write(sockfd, buf, n);
                        write(STDOUT_FILENO, buf, n);
                    }
                    else if (n == -1)
                    {
                        if (errno == ECONNRESET)
                        { //收到重置标志
                            close(sockfd);
                            client[i].fd = -1; //
                        }
                        else
                            geterror("read error");
                    }
                    if (ret == 1)
                        break; //跳出for
                }
            }


            }
        }
    }
    close(cfd);
    return 0;
}
