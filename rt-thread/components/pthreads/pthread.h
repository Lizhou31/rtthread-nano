/**
 * @file pthread.h
 * @author Lizhou
 * @brief a pthread features implementation test
 * @version 0.1
 * @date 2023-10-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _PTHREAD_H_
#define _PTHREAD_H_

#include <rtthread.h>
#include "rthw.h"
#include <errno.h>
#include "sys/types.h"
#include "sys/time.h"

#define PTHREAD_COND_INITIALIZER \
    {                            \
        -1                       \
    }
#define PTHREAD_RWLOCK_INITIALIZER \
    {                              \
        -1                         \
    }
#define PTHREAD_MUTEX_INITIALIZER \
    {                             \
        -1                        \
    }

typedef long pthread_t;
typedef long pthread_condattr_t;
typedef long pthread_rwlockattr_t;
typedef long pthread_mutexattr_t;

enum
{
    PTHREAD_MUTEX_NORMAL = 0,
    PTHREAD_MUTEX_RECURSIVE = 1,
    PTHREAD_MUTEX_ERRORCHECK = 2,
    PTHREAD_MUTEX_ERRORCHECK_NP = PTHREAD_MUTEX_ERRORCHECK,
    PTHREAD_MUTEX_RECURSIVE_NP = PTHREAD_MUTEX_RECURSIVE,
    PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
};

#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED 1

struct pthread_mutex
{
    pthread_mutexattr_t attr;
    struct rt_mutex lock;
};
typedef struct pthread_mutex pthread_mutex_t;

struct pthread_cond
{
    pthread_condattr_t attr;
    struct rt_semaphore sem;
};
typedef struct pthread_cond pthread_cond_t;

struct pthread_rwlock
{
    pthread_rwlockattr_t attr;

    pthread_mutex_t rw_mutex;      /* basic lock on this struct */
    pthread_cond_t rw_condreaders; /* for reader threads waiting */
    pthread_cond_t rw_condwriters; /* for writer threads waiting */

    int rw_nwaitreaders; /* the number of reader threads waiting */
    int rw_nwaitwriters; /* the number of writer threads waiting */
    int rw_refcount;     /* 0: unlocked, -1: locked by writer, > 0 locked by n readers */
};
typedef struct pthread_rwlock pthread_rwlock_t;

/* pthread mutex interface */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_getprioceiling(const pthread_mutex_t *mutex, int *prioceiling);
int pthread_mutex_setprioceiling(pthread_mutex_t *mutex, int prioceiling, int *old_ceiling);

int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
int pthread_mutexattr_getpshared(pthread_mutexattr_t *attr, int *pshared);
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr, int *prioceiling);
int pthread_mutexattr_setprioceiling(const pthread_mutexattr_t *attr, int prioceiling);
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol);
int pthread_mutexattr_setprotocol(const pthread_mutexattr_t *attr, int protocol);

/* pthread condition interface */
int pthread_condattr_destroy(pthread_condattr_t *attr);
int pthread_condattr_init(pthread_condattr_t *attr);

/* ADVANCED REALTIME feature in IEEE Std 1003.1, 2004 Edition */
int pthread_condattr_getclock(const pthread_condattr_t *attr,
                              clockid_t *clock_id);
int pthread_condattr_setclock(pthread_condattr_t *attr,
                              clockid_t clock_id);

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond,
                           pthread_mutex_t *mutex,
                           const struct timespec *abstime);

/* pthread rwlock interface */
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);

int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
#endif /* _PTHREAD_H_ */