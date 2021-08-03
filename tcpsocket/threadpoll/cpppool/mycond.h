#pragma once
#include <iostream>
#include <pthread.h>

class Condition_variable
{

public:
    Condition_variable()
    {
        if (pthread_cond_init(&cond, nullptr) != 0)
            throw std::exception();
    }
    ~Condition_variable()
    {
        pthread_cond_destroy(&cond);
    }

    bool notice_one()
    {
        return pthread_cond_signal(&cond) == 0;
    }
    bool notice_all()
    {
        return pthread_cond_broadcast(&cond) == 0;
    }
    bool wait(pthread_mutex_t  &mtx)
    {
        return pthread_cond_wait(&cond,&mtx)==0;
    }
    bool destroy() {
        return pthread_cond_destroy(&cond)==0;
    }

private:
    pthread_cond_t cond;

};