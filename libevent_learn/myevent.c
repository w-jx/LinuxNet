// File Name: event.c
// Author: wenjx
// Created Time: 2020年12月31日 星期四 16时26分47秒

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
//event 测试
void geterror(char *s){
     perror(s);
     exit(-1) ;
}
//event 简单函数测试
void test01()
{
    struct event_base *base = event_base_new();
    // const char **str = event_get_supported_methods();
    // for(int i=0;i<10;i++) {
    //     if (str[i]==NULL)
    //         break;
    //     printf("support %s \t", str[i]);
    // }
    // printf("\n");
    //查看目前使用的是什么
    const char *s = event_base_get_method(base);

    printf("current method %s \n",s);
    //free(s);//报错，const 指针无法释放

}
//创建事件
void test02()
{
    event_new
}
int main(){

    return 0;
}
