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
#define SERV_PORT 9998
//udp server
int main()
{
    int sockfd;
    char buf[BUFSIZ];
    char str[INET_ADDRSTRLEN];
    int i, n;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr, clit_addr;
    bzero(&serv_addr, sizeof(serv_addr)); //将结构体清零

    socklen_t clit_addr_len;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //自

    int opt=1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void*)&opt,sizeof(opt));
    
    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    //这个服务器端的bind是不可少的，不管是UDP还是TCP
    printf("wait for connecting...\n");

    while (1)
    {
        clit_addr_len = sizeof(clit_addr);
        n = recvfrom(sockfd, buf, BUFSIZ, 0, (struct sockadr *)&clit_addr, &clit_addr_len);
        if (n == -1)
        {
            perror("recvfrom error");
        }
        printf("recv from ip:%s,and port:%d \n", inet_ntop(AF_INET, &clit_addr.sin_addr, str, sizeof(str)),
               ntohs(clit_addr.sin_port));
        write(STDOUT_FILENO,buf,n);
        for (i = 0; i < n; i++)
        {
            buf[i] = toupper(buf[i]);
        }
        n = sendto(sockfd, buf, n, 0, (struct sockaddr *)&clit_addr, sizeof(clit_addr));
        if (n == -1)
            perror("sendto error");
    }
    close(sockfd);
}
