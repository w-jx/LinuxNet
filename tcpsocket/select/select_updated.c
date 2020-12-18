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
#define SERV_PORT 9999
//select方式实现的server ,自己打造数组，将监听的整合进来
int main()
{
    int lfd = 0;
    int cfd = 0;
    int sockfd,maxfd;
    int i = 0;
    int ret = 0;
    int n=0;
    int client[FD_SETSIZE]; //宏是1024
    int maxi = -1;
    char buf[BUFSIZ];
    char str[INET_ADDRSTRLEN]; //内核 自带的宏16,网络地址结构的长度
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

    maxfd = lfd;          //最大的文件描述符
    fd_set rset, allset;  //读文件描述符集，和临时变量备份
    FD_ZERO(&allset);     //清空监听集合
    FD_SET(lfd, &allset); //将待监听的fd添加到监听集合中

    for (i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;
    while (1)
    {
        rset = allset;
        ret = select(maxfd + 1, &rset, NULL, NULL, NULL); //使用select 监听
        if (ret < 0)
            geterror("select error");
        if (ret > 0)
        { //说明有客户端连接
            if (FD_ISSET(lfd, &rset))
            { //lfd满足监听的读事件
                clit_addr_len = sizeof(clit_addr);
                cfd = Accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);
                //printf("a new client from ip=%s,port=%d\n", inet_net_ntop(AF_INET, &clit_addr.sin_addr,str, sizeof(str)), ntohs(clit_addr.sin_port));
                //注意这个accept()不会阻塞
                for (i = 0; i < FD_SETSIZE; i++)
                {
                    if (client[i] < 0)
                    {
                        client[i] = cfd;
                        break;
                    }
                }//for
                if (i == FD_SETSIZE)
                {
                    fputs("too many clients\n", stderr);
                    exit(1);
                }
                FD_SET(cfd, &allset); //将新产生的fd添加到监听集合中，监听数据读事件
                if (maxfd < cfd)
                    maxfd = cfd;
                if (i > maxi)
                    maxi = i; //保证maxi存储的总是client[]最大的下标
                if (ret == 1)
                    continue;
            }
            for (i = 0; i <= maxi; i++)
            {
                if ((sockfd = client[i]) < 0)
                    continue;
                if (FD_ISSET(sockfd, &rset))
                { //找到满足读事件的fd
                    if ((n = read(sockfd, buf, sizeof(buf))) == 0)
                    { //客户端关闭连接了，服务器端也应该关闭对应连接
                        close(sockfd);
                        FD_CLR(sockfd, &allset); //将关闭的fd,移除监听集合
                        client[i] = -1;
                    }
                    else if (n == -1)
                        geterror("read error");
                    else if (n > 0)
                    {
                        for (int j = 0; j < n; j++)
                            buf[j] = toupper(buf[j]);
                        write(sockfd, buf, n);
                        write(STDOUT_FILENO, buf, n);
                    }
                    if (ret == 0)
                        break; //跳出for
                }//sockfd在监听集合中
            }//for循环，i<=maxi
        }//if (ret>0)
    }//while(1)
    close(cfd);
    return 0;
}
