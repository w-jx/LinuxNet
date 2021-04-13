/*
 * @Author: your name
 * @Date: 2021-04-13 17:08:02
 * @LastEditTime: 2021-04-13 20:29:22
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /threadpoll/cpppool/mylocker.h
 */
#pragma once
#include <iostream>
#include <pthread.h>

class Locker
{

public:
    Locker()
    {
        if(pthread_mutex_init(&this->mtx,nullptr) !=0)
                throw std::exception();        
    }
    ~Locker()
    {
        pthread_mutex_destroy(&this->mtx);
    }
    bool lock() {
        return pthread_mutex_lock(&mtx) ==0;  
    }
    bool unlock() {
        return pthread_mutex_unlock(&mtx)==0;
    }
    bool destroy() {
        return pthread_mutex_destroy(&mtx)==0;
    }
    pthread_mutex_t& get()  {
        return this->mtx;
    }
    pthread_mutex_t& operator*()  {
        return this->mtx;
    }

private:
      pthread_mutex_t mtx; 
};