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
#include<sys/socket.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<strings.h>
#define SERV_PORT 9999
//select方式实现的server 
int main(){
    int lfd=0;
    int cfd=0;
    int maxfd = 0;//最大的文件描述符
    int i = 0;
    int ret=0;
    int n;
    char buf[BUFSIZ],cliet_ip[1024];
    lfd = Socket(AF_INET,SOCK_STREAM,0);

    int opt =1;//设置端口复用
    setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,(void*)&opt,sizeof(opt));

    struct sockaddr_in serv_addr,clit_addr;
    bzero(&serv_addr,sizeof(serv_addr));//将结构体清零
    socklen_t clit_addr_len;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//自动匹配有效IP
    Bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    Listen(lfd,128);

    maxfd = lfd;//最大的文件描述符
    fd_set rset,allset;//读文件描述符集，和临时变量备份
    FD_ZERO(&allset);//清空监听集合
    FD_SET(lfd,&allset);//将待监听的fd添加到监听集合中

    while(1) {
        rset = allset;
        ret = select(maxfd+1,&rset,NULL,NULL,NULL);//使用select 监听
        if (ret<0) 
            geterror("select error");
        if (ret>0)   {//说明有客户端连接
            if( FD_ISSET(lfd,&rset) ) {//lfd满足监听的读事件
                clit_addr_len = sizeof(clit_addr);
                cfd = Accept(lfd,(struct sockaddr*)&clit_addr,&clit_addr_len);
                //注意这个accept()不会阻塞
                FD_SET(cfd,&allset);//将新产生的fd添加到监听集合中，监听数据读事件
                if (maxfd<cfd)
                    maxfd=cfd;
                if (ret==1) //说明select 只返回一个，是lfd,后面程序无需执行
                    continue;
            }
            for(i=lfd+1;i<maxfd+1;i++)  {//注意一定是maxfd+1，否则程序出错
                if (FD_ISSET(i,&rset) )  {//找到满足读事件的fd
                    if ((n = read(i, buf, sizeof(buf))) == 0)
                    { //客户端关闭连接了，服务器端也应该关闭对应连接
                        close(i);
                        FD_CLR(i, &allset);//将关闭的fd,移除监听集合
                    }
                    else if (n == -1)
                        geterror("read error");
                    else if (n > 0)
                    {
                        for (int j = 0; j < n; j++)
                            buf[j] = toupper(buf[j]);
                        write(i, buf, n);
                    }//n>0
                }//if(FD_ISSET(i,&rset))
            }//for(i=lfd+1....)
        }//if(ret>0)
    }//while(1)
    close(cfd);
    return 0;
}
