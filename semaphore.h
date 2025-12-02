#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <pthread.h>
#include "base_lock.h"

/**
 * 信号量结构
 */
typedef struct {
    int value;                    // 信号量的值
    base_lock_t lock;             // 保护信号量结构
    pthread_cond_t cond;          // 条件变量（用于线程等待/唤醒）
    const char *name;             // 信号量名称（调试用）

    // 统计信息（用于实验验证）
    int  wait_count;              // 当前等待的线程数
    long total_wait_count;        // 累计等待次数
    long total_signal_count;      // 累计 signal 次数
} semaphore_t;

/**
 * 初始化信号量
 * @param sem 信号量指针
 * @param value 初始值
 * @param name 信号量名称
 */
void sem_init(semaphore_t *sem, int value, const char *name);

/**
 * P 操作（wait/down）
 * 将信号量的值减 1；如果结果 < 0/资源不可用，则线程阻塞
 * @param sem 信号量指针
 */
void sem_wait(semaphore_t *sem);

/**
 * V 操作（signal/up）
 * 将信号量的值加 1；如果有等待的线程，则唤醒一个
 * @param sem 信号量指针
 */
void sem_signal(semaphore_t *sem);

/**
 * 获取信号量当前值（仅用于调试和显示）
 * @param sem 信号量指针
 * @return 当前信号量值
 */
int sem_get_value(semaphore_t *sem);

/**
 * 打印信号量状态（用于调试）
 * @param sem 信号量指针
 */
void sem_print_status(semaphore_t *sem);

/**
 * 销毁信号量（释放内部资源）
 * @param sem 信号量指针
 */
void sem_destroy(semaphore_t *sem);

#endif // SEMAPHORE_H


