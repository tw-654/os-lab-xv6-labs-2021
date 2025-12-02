#ifndef BASE_LOCK_H
#define BASE_LOCK_H

#include <pthread.h>
#include <stdbool.h>

/**
 * 基础锁结构
 * 注意：这是框架提供的基础设施，仅用于保护你实现的数据结构
 */
typedef struct {
    pthread_mutex_t internal_mutex;
    const char *name;  // 用于调试
} base_lock_t;

/**
 * 初始化基础锁
 * @param lock 锁指针
 * @param name 锁的名称（用于调试输出）
 */
void base_lock_init(base_lock_t *lock, const char *name);

/**
 * 获取锁
 * @param lock 锁指针
 */
void base_lock_acquire(base_lock_t *lock);

/**
 * 释放锁
 * @param lock 锁指针
 */
void base_lock_release(base_lock_t *lock);

/**
 * 销毁锁
 * @param lock 锁指针
 */
void base_lock_destroy(base_lock_t *lock);

#endif // BASE_LOCK_H


