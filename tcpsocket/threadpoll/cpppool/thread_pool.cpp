/*
 * @Author: your name
 * @Date: 2021-04-12 21:30:48
 * @LastEditTime: 2021-04-13 21:11:51
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /threadpoll/cpppool/thread_pool.cpp
 */
#include <iostream>
//#include "thread_pool.h" //测试没问题了
#include "thread_pool3.h" //测试没问题了
using namespace std;
/* 线程池中的线程，模拟处理业务 */
void *process(void *arg)
{
    printf("thread 0x%x working on task %d\n ",(unsigned int)pthread_self(),*(int *)arg);
    sleep(1);
    printf("task %d is end\n",*(int *)arg);

    return NULL;
}

int main()
{

    Thread_pool pool(3, 100, 100);
    int num[20], i;
    for (i = 0; i < 20; i++)
    {
        num[i] = i;
        printf("add task %d\n", i);
        pool.add(process,(void*)&num[i]);      /* 向线程池中添加任务 */
    }
    sleep(8);       
    return 0;
}
