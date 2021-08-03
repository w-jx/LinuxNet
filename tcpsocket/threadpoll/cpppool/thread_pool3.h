/*
 * @Author: your name
 * @Date: 2021-04-13 20:03:49
 * @LastEditTime: 2021-04-13 21:17:18
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /threadpoll/cpppool/thread_pool3.h
 */
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <functional>
#include <signal.h>
#include <vector>
#include <queue>
#include "mylocker.h"
#include "mycond.h"

using namespace std;

using myfunction = std::function<void *(void *)>;

/*
thread_pool.h 的重新构建
1.利用queue替换  原来的数组实现的队列

*/

using threadpool_task_t = struct
{
    myfunction function; /* 函数指针，回调函数 */
    void *arg;           /* 上面函数的参数 */
};

class Thread_pool
{
public:
    Thread_pool() = default;
    /**
     * @description: Thread_pool 构造
     * @param {int} min_thr_num 线程池最小线程个数
     * @param {int} max_thr_num 线程池最大线程个数
     * @return {*}
     */
    explicit Thread_pool(int min_thr_num, int max_thr_num, int max_task_num);
    ~Thread_pool();
    /**
     * @description: 向线程池中添加一个任务
     * @param {myfunction} 任务 函数
     * @param {void} *arg  任务函数参数
     * @return {*}
     */
    void add(myfunction func, void *arg);

    Locker mylock;                      /* 用于锁住本结构体 */
    Locker thread_counter;              /* 记录忙状态线程个数的锁 -- busy_thr_num */
    Condition_variable queue_not_full;  /* 当任务队列满时，添加任务的线程阻塞，等待此条件变量 */
    Condition_variable queue_not_empty; /* 任务队列里不为空时，通知等待任务的线程 */

    //vector<pthread_t *> threads;       //存放线程池
    pthread_t *threads;
    // unordered_map<pthread_t*,bool> hashmap;//线程池？

    pthread_t adjust_tid;              //管理线程
    queue<threadpool_task_t> task_que; //任务队列

    int min_thr_num;       /* 线程池最小线程数 */
    int max_thr_num;       /* 线程池最大线程数 */
    int live_thr_num;      /* 当前存活线程个数 */
    int busy_thr_num;      /* 忙状态线程个数 */
    int wait_exit_thr_num; /* 要销毁的线程个数 */

    int max_task_num; //最大任务数

    bool isClosed; //标志位

private:
    /**
     * @description:线程池工作线程 
     * @param {void} *arg 线程池工作 参数传递
     * @return {*}
     */
    static void *threadpool_thread(void *arg);
    /**
     * @description: 调整 线程
     * @param {void} *arg 调整线程 参数传递
     * @return {*}
     */
    static void *adjust_thread(void *arg);

    bool is_thread_alive(pthread_t &tid)
    {
        return pthread_kill(tid, 0) != ESRCH; ////发0号信号，测试线程是否存活
    }
};














Thread_pool::Thread_pool(int min_thr_num, int max_thr_num, int max_task_num) : min_thr_num(min_thr_num), max_thr_num(max_thr_num),
                                                                               max_task_num(max_task_num)
{
    this->busy_thr_num = 0;           //初始时 忙线程个数为0
    this->live_thr_num = min_thr_num; //当前存活线程个数 初始是最小线程个数
    this->isClosed = false;           //线程池不处于关闭状态

    threads = new pthread_t[max_thr_num];

    for (int i = 0; i < min_thr_num; ++i)
    { //开启min_thr_num个线程，初始化线程池中线程
        pthread_create(&threads[i], nullptr, threadpool_thread, this);
    }
    pthread_create(&this->adjust_tid, nullptr, adjust_thread, this);
}

Thread_pool::~Thread_pool()
{
}

void Thread_pool::add(myfunction func, void *arg)
{

    mylock.lock();
    while (task_que.size() == max_task_num && !isClosed)
        queue_not_full.wait(*mylock);

    if (isClosed) //注意这边的解锁？
        mylock.unlock();

    threadpool_task_t task;
    task.function = func;
    task.arg = arg;

    this->task_que.push(task);
    queue_not_empty.notice_one();
    mylock.unlock();
}

void *Thread_pool::threadpool_thread(void *arg)
{
    Thread_pool *pool = static_cast<Thread_pool *>(arg);

    threadpool_task_t task;
    while (true)
    {
        while (pool->task_que.size() == 0 && !pool->isClosed)
        {
            printf("thread 0x%x is waiting\n", (unsigned int)pthread_self());
            pool->queue_not_empty.wait(pool->mylock.get());

            if (pool->wait_exit_thr_num > 0)
            { //x
                /*如果线程池里线程个数大于最小值时可以结束当前线程*/
                if (pool->live_thr_num > pool->min_thr_num)
                {
                    printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
                    pool->live_thr_num--;
                    pool->mylock.unlock();
                    pthread_exit(NULL); //结束当前线程
                }
            } //if >0
        }     //while
        //线程池关了 需要关闭所有的线程，让他们自己退出
        if (pool->isClosed)
        {
            pool->mylock.unlock();
            printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
            pthread_exit(NULL); /* 线程自行结束 */
        }
        //从任务队列取出 任务
        task = pool->task_que.front();
        pool->task_que.pop();
        //任务取出后，通知可以添加新的任务
        pool->queue_not_full.notice_all();
        //任务取出，将线程池解锁
        pool->mylock.unlock();

        printf("thread 0x%x start working\n", (unsigned int)pthread_self());
        pool->thread_counter.lock();
        pool->busy_thr_num++;
        pool->thread_counter.unlock();

        task.function(task.arg); //执行回调函数

        printf("thread 0x%x end working\n", (unsigned int)pthread_self());
        pool->thread_counter.lock();
        pool->busy_thr_num--; //处理掉一个线程，忙线程数--
        pool->thread_counter.unlock();
    } //while (true)
    pthread_exit(nullptr);
    return nullptr;
}
void *Thread_pool::adjust_thread(void *arg)
{
    Thread_pool *pool = (Thread_pool *)(arg);
    while (true)
    {
        sleep(10); //每10秒 管理一次

        pool->mylock.lock();
        int que_size = pool->task_que.size();  //任务个数
        int live_thr_num = pool->live_thr_num; //存活线程个数
        pool->mylock.unlock();

        pool->thread_counter.lock();
        int busy_thr_num = pool->busy_thr_num;
        pool->thread_counter.unlock();

        //创建新线程 任务数量大于10 存活线程个数小于最大线程数
        if (que_size > 10 && live_thr_num < pool->max_task_num)
        {
            pool->mylock.lock();
            for (int i = 0; i < pool->max_task_num; ++i)
            {
                if (pool->threads[i] == 0 && pool->is_thread_alive(pool->threads[i]))
                {
                    pthread_create(&pool->threads[i], nullptr, threadpool_thread, arg);
                    ++pool->live_thr_num;
                }

            } //for
            pool->mylock.unlock();
        } //if

        //销毁多余的空闲线程
        if (busy_thr_num * 2 < live_thr_num && live_thr_num > pool->min_thr_num)
        {
            pool->mylock.lock();
            pool->wait_exit_thr_num = 10;
            pool->mylock.unlock();
            for (int i = 0; i < 10; ++i)
            {
                pool->queue_not_empty.notice_one();
            }
        }
    } //while (true)
    return nullptr;
}