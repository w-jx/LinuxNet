#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H
#endif
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <functional>
#include <signal.h>
#include "mylocker.h"
#include "mycond.h"

int const DEFAULT_TIME = 10;        /*10s检测一次*/
int const MIN_WAIT_TASK_NUM = 10;   /*如果queue_size > MIN_WAIT_TASK_NUM 添加新的线程到线程池*/
int const DEFAULT_THREAD_VARY = 10; /*每次创建和销毁线程的个数*/

using namespace std;

using myfunction = std::function<void *(void *)>;

// typedef struct
// {
//     myfunction function; /* 函数指针，回调函数 */
//     void *arg;           /* 上面函数的参数 */
// } threadpool_task_t;     /* 各子线程任务结构体 */

using threadpool_task_t =struct
{
    myfunction function; /* 函数指针，回调函数 */
    void *arg;           /* 上面函数的参数 */
} ; 

class Thread_pool
{

public:
    Thread_pool() = default;
    explicit Thread_pool(int min_thr_num, int max_thr_num, int queue_max_size)
    {

        this->min_thr_num = min_thr_num; //传参
        this->max_thr_num = max_thr_num; //传参
        this->busy_thr_num = 0;
        this->live_thr_num = min_thr_num;      /* 活着的线程数 初值=最小线程数 */
        this->queue_size = 0;                  /* 有0个产品 */
        this->queue_max_size = queue_max_size; //传参
        this->queue_front = 0;
        this->queue_rear = 0;
        this->IsShutDown = false; /* 不关闭线程池 */

        this->threads = new pthread_t[this->max_thr_num]; //线程数组开辟到堆          上

        this->task_queue = new threadpool_task_t[this->queue_max_size]; //任务队列开辟空间

        //初始化锁，初始化条件变量 、//不需要了

        for (int i = 0; i < this->min_thr_num; i++)
        {
            pthread_create(&this->threads[i], nullptr, threadpool_thread, this); //参数3用std::function包装的无法传递
        }
        pthread_create(&this->adjust_tid, nullptr, adjust_thread, this);
    }

    /*向线程池添加任务*/
    void add(myfunction func, void *arg)
    {
        mylock.lock();
        //任务队列满了，则队列满条件变量 wait阻塞
        while (this->queue_size == this->queue_max_size && !this->IsShutDown)
        {
            queue_not_full.wait(mylock.get());
        }
        if (this->IsShutDown)
            mylock.unlock();

        //清空 工作线程调用的回调函数的参数
        if (this->task_queue[this->queue_rear].arg != nullptr)
        {
            //delete this->task_queue[this->queue_rear].arg;
            this->task_queue[this->queue_rear].arg = nullptr;
        }
        //添加任务到条件队列

        this->task_queue[this->queue_rear].function = func;
        this->task_queue[this->queue_rear].arg = arg;
        this->queue_rear = (this->queue_rear + 1) % this->queue_max_size; /* 队尾指针移动, 模拟环形 */
        this->queue_size++;                                               //实际任务数 ++

        queue_not_empty.notice_one();
        mylock.unlock();
    }
    ~Thread_pool()
    {

        this->IsShutDown = true;
        pthread_join(this->adjust_tid, nullptr);
        for (int i = 0; i < this->live_thr_num; i++)
        {
            /*通知所有的空闲线程*/
            this->queue_not_empty.notice_one();
        }
        this->IsShutDown = true;
        for (int i = 0; i < this->live_thr_num; i++)
        {
            pthread_join(this->threads[i], nullptr);
        }

        if (this->task_queue)
        {
            delete[] task_queue;
            task_queue = nullptr;
        }
        if (this->threads)
        {
            delete[] this->threads;
            this->threads = nullptr;
            this->mylock.lock();
            this->mylock.destroy();
            this->thread_counter.lock();
            this->thread_counter.destroy();
            this->queue_not_empty.destroy();
            this->queue_not_full.destroy();
        }
    }

public:
    Locker mylock;                      /* 用于锁住本结构体 */
    Locker thread_counter;              /* 记录忙状态线程个数的锁 -- busy_thr_num */
    Condition_variable queue_not_full;  /* 当任务队列满时，添加任务的线程阻塞，等待此条件变量 */
    Condition_variable queue_not_empty; /* 任务队列里不为空时，通知等待任务的线程 */

    pthread_t *threads;            /* 存放线程池中每个线程的tid。数组 */
    pthread_t adjust_tid;          /* 存管理线程tid */
    threadpool_task_t *task_queue; /* 任务队列 */

    int min_thr_num;       /* 线程池最小线程数 */
    int max_thr_num;       /* 线程池最大线程数 */
    int live_thr_num;      /* 当前存活线程个数 */
    int busy_thr_num;      /* 忙状态线程个数 */
    int wait_exit_thr_num; /* 要销毁的线程个数 */

    int queue_front;    /* task_queue队头下标 */
    int queue_rear;     /* task_queue队尾下标 */
    int queue_size;     /* task_queue队中实际任务数 */
    int queue_max_size; /* task_queue队列可容纳任务数上限 */

    bool IsShutDown; /* 标志位，线程池使用状态，true或false */

private:
    static void *threadpool_thread(void *arg) //工作线程
    {
        Thread_pool *pool = (Thread_pool *)(arg);
        threadpool_task_t task;

        while (true)
        {
            pool->mylock.lock(); //刚创建线程，没任务，阻塞在"队列不为空"条件变量上

            while (pool->queue_size == 0 && !pool->IsShutDown)
            {
                printf("thread 0x%x is waiting\n", (unsigned int)pthread_self());
                pool->queue_not_empty.wait(pool->mylock.get());

                if (pool->wait_exit_thr_num > 0)
                {
                    /*如果线程池里线程个数大于最小值时可以结束当前线程*/
                    if (pool->live_thr_num > pool->min_thr_num)
                    {
                        printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
                        pool->live_thr_num--;
                        pool->mylock.unlock();
                        pthread_exit(NULL);
                    }
                } // if >0
            }     // while ==0

            if (pool->IsShutDown)
            { //线程池关了，需要关闭所有的线程，让他们自己退出
                pool->mylock.unlock();
                printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
                pthread_exit(NULL); /* 线程自行结束 */
            }

            /*从任务队列里获取任务, 是一个出队操作*/
            task.function = pool->task_queue[pool->queue_front].function;
            task.arg = pool->task_queue[pool->queue_front].arg;

            pool->queue_front = (pool->queue_front + 1) % pool->queue_max_size; /* 出队，模拟环形队列 */
            pool->queue_size--;

            //任务取出后，通知可以添加新任务
            pool->queue_not_full.notice_all();
            //任务取出，可以将线程池解锁
            pool->mylock.unlock();

            //执行任务
            printf("thread 0x%x start working\n", (unsigned int)pthread_self());
            pool->thread_counter.lock();
            pool->busy_thr_num++;
            pool->thread_counter.unlock();

            task.function(task.arg); //执行回调函数

            //执行结束后
            printf("thread 0x%x end working\n", (unsigned int)pthread_self());
            pool->thread_counter.lock();
            pool->busy_thr_num--; //处理掉一个线程，忙线程数--
            pool->thread_counter.unlock();
        }
        pthread_exit(nullptr);
        return nullptr;
    }

    bool is_thread_alive(pthread_t tid)
    {
        int kill_rc = pthread_kill(tid, 0); //发0号信号，测试线程是否存活
        if (kill_rc == ESRCH)
        {
            return false;
        }

        return true;
    }
    static void *adjust_thread(void *arg) //管理线程
    {
        volatile int i = 0;
        Thread_pool *pool = (Thread_pool *)(arg);

        while (!pool->IsShutDown)
        {
            sleep(DEFAULT_TIME); //定时管理

            pool->mylock.lock();
            int queue_size = pool->queue_size;     /* 关注 任务数 */
            int live_thr_num = pool->live_thr_num; /* 存活 线程数 */
            pool->mylock.unlock();

            pool->thread_counter.lock();
            int busy_thr_num = pool->busy_thr_num;
            pool->thread_counter.unlock();

            //创建新线程 算法： 任务数大于最小线程池个数, 且存活的线程数少于最大线程个数时 如：30>=10 && 40<100*/
            if (queue_size >= MIN_WAIT_TASK_NUM && live_thr_num < pool->max_thr_num)
            {
                pool->mylock.lock();
                int add{0};

                /*一次增加 DEFAULT_THREAD 个线程*/
                for (i = 0; i < pool->max_thr_num && add < DEFAULT_THREAD_VARY && pool->live_thr_num < pool->max_thr_num; i++)
                {
                    if (pool->threads[i] == 0 || !pool->is_thread_alive(pool->threads[i]))
                    {
                        pthread_create(&(pool->threads[i]), nullptr, threadpool_thread, arg);
                        add++;
                        pool->live_thr_num++;
                    }
                } //for
                pool->mylock.unlock();
            }

            //销毁多余的空闲线程  算法：忙线程X2 小于 存活的线程数 且 存活的线程数 大于 最小线程数时*/
            if (busy_thr_num * 2 < live_thr_num && live_thr_num > pool->min_thr_num)
            {

                pool->mylock.lock();
                pool->wait_exit_thr_num = DEFAULT_THREAD_VARY;
                pool->mylock.unlock();
                for (i = 0; i < DEFAULT_THREAD_VARY; i++)
                {
                    pool->queue_not_empty.notice_one();
                }
            }
        }

        return nullptr;
    }
};