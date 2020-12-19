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
//非阻塞的套接字 测试用的客户端
int main()
{

    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    int sockfd, i;
    char ch = 'a';

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);

    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    while (1)
    {
        for (i = 0; i < MAXLINE / 2; i++)
            buf[i] = ch;
        buf[i - 1] = '\n'; //aaaaa\n
        ch++;
        for (; i < MAXLINE; i++)
            buf[i] = ch;
        buf[i - 1] = '\n'; //BBBBB\n
        ch++;
        write(sockfd, buf, sizeof(buf));
        sleep(5);
    }
    close(sockfd);
    return 0;
}