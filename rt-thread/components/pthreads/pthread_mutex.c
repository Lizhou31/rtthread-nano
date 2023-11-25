/**
 * @file pthread_mutex.c
 * @author Lizhou
 * @brief 
 * @version 0.1
 * @date 2023-10-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <rtthread.h>
#include "pthread.h"

#define MUTEXATTR_SHARED_MASK 0x0010
#define MUTEXATTR_TYPE_MASK 0x000f

const pthread_mutexattr_t pthread_default_mutexattr = PTHREAD_PROCESS_PRIVATE;

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    rt_err_t result;
    char mutex_name[RT_NAME_MAX];
    static rt_uint16_t pthread_mutex_number = 0;

    /* check pointer not null */
    if (!mutex)
        return EINVAL;

    /* parameter check */
    if (attr == RT_NULL)
        mutex->attr = pthread_default_mutexattr;
    else
        mutex->attr = *attr;

    /* build mutex name */
    rt_snprintf(mutex_name, sizeof(mutex_name), "pmtx%02d", pthread_mutex_number++);

    /* init mutex lock */
    result = rt_mutex_init(&(mutex->lock), mutex_name, RT_IPC_FLAG_PRIO);
    if (result != RT_EOK)
        return EINVAL;

    /* detach the object from system object container */
    rt_object_detach(&(mutex->lock.parent.parent));
    mutex->lock.parent.parent.type = RT_Object_Class_Mutex;

    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    if (!mutex || mutex->attr == -1)
    {
        return EINVAL;
    }

    if (mutex->lock.owner != RT_NULL)
        return EBUSY;

    rt_memset(mutex, 0, sizeof(pthread_mutex_t));
    mutex->attr = -1;

    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    int mtype;
    rt_err_t result;

    if (!mutex)
        return EINVAL;

    if (mutex->attr == -1)
    {
        /* init mutex */
        pthread_mutex_init(mutex, RT_NULL);
    }

    mtype = mutex->attr & MUTEXATTR_TYPE_MASK;
    rt_enter_critical();
    if (mutex->lock.owner == rt_thread_self() &&
        mtype != PTHREAD_MUTEX_RECURSIVE)
    {
        rt_exit_critical();
        return EDEADLK;
    }
    rt_exit_critical();

    result = rt_mutex_take(&(mutex->lock), RT_WAITING_FOREVER);
    if (result == RT_EOK)
        return 0;

    return EINVAL;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    rt_err_t result;
    if (!mutex)
        return EINVAL;
    if (mutex->attr == -1)
    {
        /* init mutex */
        pthread_mutex_init(mutex, RT_NULL);
    }

    if (mutex->lock.owner != rt_thread_self())
    {
        int mtype;
        mtype = mutex->attr & MUTEXATTR_TYPE_MASK;

        /* error check return EPERM */
        if (mtype == PTHREAD_MUTEX_ERRORCHECK)
            return EPERM;

        /* no thread waiting on this mutex */
        if (mutex->lock.owner == RT_NULL)
            return 0;
    }

    result = rt_mutex_release(&(mutex->lock));
    if (result == RT_EOK)
        return 0;

    return EINVAL;
}