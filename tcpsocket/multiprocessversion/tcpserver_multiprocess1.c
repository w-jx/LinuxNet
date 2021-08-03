//#include <./wrap.h>
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
//能够回收子进程的，防止僵尸进程的出现
void catch_child(int signum)
{

    while (waitpid(0, NULL, WNOHANG) > 0)
        ;

    return;
}
int main()
{
    int lfd = 0;
    int cfd = 0;
    int ret = 0;
    pid_t pid;
    lfd = Socket(AF_INET, SOCK_STREAM, 0);

    char buf[BUFSIZ], cliet_ip[1024];
    struct sockaddr_in serv_addr, clit_addr;
    bzero(&serv_addr, sizeof(serv_addr)); //将结构体清零
    socklen_t clit_addr_len;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ret = Bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    Listen(lfd, 128);

    clit_addr_len = sizeof(clit_addr);
    //cfd =Accept(lfd,(struct sockaddr *)&clit_addr,&clit_addr_len);

    //printf("client ip=%s,port=%d\n",
    //        inet_ntop(AF_INET,&clit_addr.sin_addr.s_addr,cliet_ip,sizeof(cliet_ip)),
    //        ntohs(clit_addr.sin_port));
    //printf("char client ip=%s\n",cliet_ip);

    while (1)
    {
        cfd = Accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);
        pid = fork();
        if (pid < 0)
            geterror("fork() error");
        else if (pid == 0)
        {
            close(lfd);
            while (1)
            {
                ret = read(cfd, buf, sizeof(buf));
                if (ret == 0)
                {
                    printf("client exit");
                    close(cfd); //读不到东西意味着客户端关闭
                    exit(1);    //异常退出
                }
                write(STDOUT_FILENO, buf, ret);
                for (int i = 0; i < ret; i++)
                {

                    buf[i] = toupper(buf[i]);
                }
                write(cfd, buf, ret);
                write(STDOUT_FILENO, buf, ret);
            }
        }
        else
        { //父进程 注册SIGCHID回收
            struct sigaction act;
            act.sa_handler = catch_child;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
            ret = sigaction(SIGCHLD, &act, NULL);
            if (ret != 0)
                geterror("sigaction error");
            close(cfd);
            continue;
        }
    }
    close(lfd);
    close(cfd);
    return 0;
}
