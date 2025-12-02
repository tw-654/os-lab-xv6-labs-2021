#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "base_lock.h"
#include "semaphore.h"

#define DEBUG 1

void sem_init(semaphore_t *sem, int value, const char *name) {
    // 初始化信号量的值
    sem->value = value;

    // 初始化基础锁
    base_lock_init(&sem->lock, name);

    // 初始化条件变量
    if (pthread_cond_init(&sem->cond, NULL) != 0) {
        fprintf(stderr, "Failed to init cond for semaphore: %s\n", name);
        exit(1);
    }

    // 初始化统计信息
    sem->name = name;
    sem->wait_count = 0;
    sem->total_wait_count = 0;
    sem->total_signal_count = 0;
}

/**
 * P 操作（wait/down）
 * 若资源不可用则阻塞等待
 */
void sem_wait(semaphore_t *sem) {
    base_lock_acquire(&sem->lock);

#if DEBUG
    printf("[SEM] %s: wait enter, value=%d, tid=%ld\n",
           sem->name, sem->value, (long)pthread_self());
#endif

    // 防止虚假唤醒：必须用 while
    while (sem->value == 0) {
        sem->wait_count++;
        sem->total_wait_count++;

#if DEBUG
        printf("[SEM] %s: blocking, wait_count=%d, tid=%ld\n",
               sem->name, sem->wait_count, (long)pthread_self());
#endif

        // pthread_cond_wait 会在内部原子地：
        // 1. 释放 mutex
        // 2. 睡眠等待
        // 3. 被唤醒后重新加锁
        pthread_cond_wait(&sem->cond, &sem->lock.internal_mutex);

        sem->wait_count--;

#if DEBUG
        printf("[SEM] %s: woke up, value=%d, tid=%ld\n",
               sem->name, sem->value, (long)pthread_self());
#endif
    }

    // 有资源可用，消费一个
    sem->value--;

#if DEBUG
    printf("[SEM] %s: acquired, value=%d, tid=%ld\n",
           sem->name, sem->value, (long)pthread_self());
#endif

    base_lock_release(&sem->lock);
}

/**
 * V 操作（signal/up）
 * 资源 +1，若有等待线程则唤醒一个
 */
void sem_signal(semaphore_t *sem) {
    base_lock_acquire(&sem->lock);

    sem->value++;
    sem->total_signal_count++;

#if DEBUG
    printf("[SEM] %s: signal, value=%d, wait_count=%d, tid=%ld\n",
           sem->name, sem->value, sem->wait_count, (long)pthread_self());
#endif

    // 有等待线程则唤醒一个
    if (sem->wait_count > 0) {
        pthread_cond_signal(&sem->cond);
    }

    base_lock_release(&sem->lock);
}

int sem_get_value(semaphore_t *sem) {
    int v;
    base_lock_acquire(&sem->lock);
    v = sem->value;
    base_lock_release(&sem->lock);
    return v;
}

void sem_print_status(semaphore_t *sem) {
    base_lock_acquire(&sem->lock);
    printf("=== Semaphore %s Status ===\n", sem->name);
    printf("current value        : %d\n", sem->value);
    printf("current wait_count   : %d\n", sem->wait_count);
    printf("total wait_count     : %ld\n", sem->total_wait_count);
    printf("total signal_count   : %ld\n", sem->total_signal_count);
    base_lock_release(&sem->lock);
}

void sem_destroy(semaphore_t *sem) {
    pthread_cond_destroy(&sem->cond);
    base_lock_destroy(&sem->lock);
}


