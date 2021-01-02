// File Name: myread.c
// Author: wenjx
// Created Time: 2021年01月02日 星期六 10时41分48秒

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
#include <event2/event.h>
void geterror(char *s){
     perror(s);
     exit(-1) ;
}
void cb_read(evutil_socket_t fd,short what,void *arg)
{
char buf[1024];
int len = read(fd,buf,sizeof(buf));
write(STDOUT_FILENO,buf,len);
sleep(1);     
}
int main(){

unlink("wjxfifo");
mkfifo("wjxfifo",0644);
int fd = open("wjxfifo", O_RDONLY);
if (fd == -1)
     geterror("open fifo error");

//创建event_base
struct event_base *base = event_base_new();
//创建事件
struct event *ev = event_new(base,fd,EV_READ|EV_PERSIST,cb_read,NULL);
//添加事件到event_base
event_add(ev,NULL);
//启动事件
event_base_dispatch(base);

//销毁event_base
event_base_free(base);
event_free(ev);
return 0 ;
}
