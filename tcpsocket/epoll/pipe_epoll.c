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
#define MAXLINE 10
//epoll不止能用在套接字上，监听的是文件描述符，所以管道也可以
//这个例子是用来理解epoll的ET LT的差异
int main(int argc, char *argv[])
{

    int efd, i;
    int pfd[2]; //管道
    pid_t pid;
    char buf[MAXLINE], ch = 'a';

    pipe(pfd);
    pid = fork();

    if (pid == 0)
    {                  //子进程 写
        close(pfd[0]); //关闭读端
        while (1)
        {

            for (i = 0; i < MAXLINE / 2; i++) //一次发5个
                buf[i] = ch;
            buf[i - 1] = '\n'; //aaaaa\n
            ch++;
            for (; i < MAXLINE; i++)
                buf[i] = ch;
            buf[i - 1] = '\n'; //BBBBB\n
            ch++;
            write(pfd[1], buf, sizeof(buf));
            sleep(5);

        } //while(1)
    }     //if pid==0
    else if (pid > 0)
    { //父进程 读

        close(pfd[1]);                   //关闭写端
        struct epoll_event event;        //epoll ctl
        struct epoll_event resevent[10]; //epoll wait就绪返回event
        int res, len;

        efd = epoll_create(10);
        //event.events=EPOLLIN|EPOLLET;//ET 边沿触发
        event.events = EPOLLIN; //LT 水平触发是默认的
        event.data.fd = pfd[0];
        epoll_ctl(efd, EPOLL_CTL_ADD, pfd[0], &event); //添加到监听队列中
        while (1)
        {

            res = epoll_wait(efd, resevent, 10, -1);
            printf("res %d\n", res); //我们都知道这边就一个管道读写，所以下标从0开始
            if (resevent[0].data.fd = pfd[0])
            {
                len = read(pfd[0], buf, MAXLINE / 2);
                write(STDOUT_FILENO, buf, len);
            } //fd=pfd[0]

        } //while(1)
        close(pfd[0]);
        close(efd);
    } //pid>0
    else if (pid < 0)
    {
        geterror("fork error");
    } //pid<0 error
}
