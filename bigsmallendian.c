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

//这个文件是判断大端小端的方法

void judge()
{
   int i=1;//0x 0 0 0 0 0 0 0 1
   //强转成char*型，这时候就取到了 i 的低地址
   char c  =  *((char *)&i);//取到了i 的低地址对应的字符 

   if(c) {
        printf("c=%d,小端\n",c);//如果是1->小端,如果是0->大端
   }
   else {
       printf("c=%d,大端\n",c);//
   }
   
}


int main()
{
    judge();
}