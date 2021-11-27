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
//udp client
int main()
{
    int sockfd;
    char buf[BUFSIZ];
    char str[INET_ADDRSTRLEN];
    int i, n;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr, clit_addr;
    bzero(&serv_addr, sizeof(serv_addr)); //将结构体清零
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    //serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //自
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    while (fgets(buf,BUFSIZ,stdin)!=NULL)
    {
        //注意是strlen，因为需要实际大小
        n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (n == -1)
            perror("sendto error");
        //n = recvfrom(sockfd, buf, BUFSIZ, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        n = recvfrom(sockfd, buf, BUFSIZ, 0, NULL,0);//不关心对端信息
        if (n == -1)
            perror("recvfrom error");

        write(STDOUT_FILENO,buf,n);
    }
    close(sockfd);
}
