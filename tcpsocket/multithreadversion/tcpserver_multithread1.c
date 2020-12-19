//#include <./wrap.h>
#include <stdio.h>
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
#include <strings.h>
#include <pthread.h>
#define SERV_PORT 9999
#define MAXLINE 8192
//上一个文件有错误，这个尝试修改下
struct s_info {//服务器地址，和套接字
    struct sockaddr_in clitaddr;//16字节
    int cfd;//4字节
};
void *do_work(void*arg) 
{
    int n,i;
    struct s_info *ts = (struct s_info*) arg;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];

    while(1)  {
        n = read(ts->cfd,buf,MAXLINE);
        if(n==0) {
            printf("the client %d is closed ..\n",ts->cfd);
            break;

        }
        printf("recv from %s at PORT %d \n",inet_ntop(AF_INET,&(*ts).clitaddr.sin_addr,str,sizeof(str)),
                ntohs( (*ts).clitaddr.sin_port));

        for(i=0;i<n;i++) 
            buf[i] = toupper(buf[i]);

        write(STDOUT_FILENO,buf,n);//回写到屏幕上
        write(ts->cfd,buf,n);//回写给客户端

    }
    close(ts->cfd);
    return (void*)0;//等同于pthread_exit(0);

}
int main(){
    int lfd=0;
    int cfd=0;
    pthread_t tid;//线程
    int i = 0;
    struct s_info ts[256];//创建结构体数组
    char buf[BUFSIZ],cliet_ip[1024];
    lfd = Socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serv_addr,clit_addr;
    bzero(&serv_addr,sizeof(serv_addr));//将结构体清零
    socklen_t clit_addr_len;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//自动匹配有效IP
    Bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    Listen(lfd,128);

    clit_addr_len = sizeof(clit_addr);

    while(1) {
        cfd =Accept(lfd,(struct sockaddr *)&clit_addr,&clit_addr_len);
        ts[i].clitaddr  =clit_addr;
        ts[i].cfd = cfd;
        pthread_create(&tid,NULL,do_work,(void*)&ts[i]);
        pthread_detach(tid);
        i++;
    }

    close(lfd);
    close(cfd);
    return 0 ;
}
