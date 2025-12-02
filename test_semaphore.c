#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // usleep
#include <pthread.h>

#include "base_lock.h"
#include "semaphore.h"

#define NUM_THREADS 10   // 线程数

semaphore_t test_sem;    // 测试用信号量
int shared_counter = 0;  // 共享计数器

void *test_thread(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 10; i++) {
        // P 操作，进入临界区
        sem_wait(&test_sem);

        int temp = shared_counter;
        usleep(1000);  // 模拟处理时间
        shared_counter = temp + 1;
        printf("[Thread %d] Counter = %d\n", id, shared_counter);

        // V 操作，离开临界区
        sem_signal(&test_sem);

        usleep(500);   // 模拟其他工作
    }

    return NULL;
}

int main() {
    printf("=== 测试信号量实现 ===\n");

    // 初始化信号量为 1（作为互斥锁使用）
    sem_init(&test_sem, 1, "test_mutex");

    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    // 创建线程
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        if (pthread_create(&threads[i], NULL, test_thread, &ids[i]) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    // 等待所有线程结束
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n最终计数器值 : %d (期望值 : %d)\n",
           shared_counter, NUM_THREADS * 10);

    sem_print_status(&test_sem);
    sem_destroy(&test_sem);

    return 0;
}


