#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "semaphore.h"
#include "base_lock.h"

// 配置参数
#define BUFFER_SIZE 10
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 2
#define ITEMS_PER_PRODUCER 20

// 物品结构
typedef struct {
    int id;              // 物品ID
    int producer_id;     // 生产者ID
    double timestamp;    // 生产时间戳
} Item;

// 有界缓冲区结构
typedef struct {
    Item buffer[BUFFER_SIZE];
    int in;              // 生产者插入位置
    int out;             // 消费者取出位置
    int count;           // 当前缓冲区中的物品数

    // 三个信号量
    semaphore_t empty;   // 空槽位数
    semaphore_t full;    // 满槽位数
    semaphore_t mutex;   // 互斥访问缓冲区
} BoundedBuffer;

// 统计信息
typedef struct {
    long produced[NUM_PRODUCERS];    // 每个生产者生产的数量
    long consumed[NUM_CONSUMERS];    // 每个消费者消费的数量
    long total_produced;
    long total_consumed;
    base_lock_t lock;                // 保护统计信息
} Statistics;

BoundedBuffer buffer;
Statistics stats;

int next_item_id = 0;
base_lock_t item_id_lock;

// 跟踪所有生产者是否已完成
int producers_finished = 0;
base_lock_t producers_finished_lock;

// 获取当前时间（秒）
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// 初始化缓冲区和信号量
void buffer_init() {
    buffer.in = 0;
    buffer.out = 0;
    buffer.count = 0;

    // empty 初始为 BUFFER_SIZE（所有槽位都是空的）
    sem_init(&buffer.empty, BUFFER_SIZE, "empty_sem");
    // full 初始为 0（还没有物品）
    sem_init(&buffer.full, 0, "full_sem");
    // mutex 初始为 1（互斥锁）
    sem_init(&buffer.mutex, 1, "buffer_mutex");

    // 初始化统计信息
    stats.total_produced = 0;
    stats.total_consumed = 0;
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        stats.produced[i] = 0;
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        stats.consumed[i] = 0;
    }
}

// 生产函数
void produce(Item item) {
    // 1. 等待空槽位
    sem_wait(&buffer.empty);   // P(empty)

    // 2. 获取互斥锁
    sem_wait(&buffer.mutex);   // P(mutex)

    // 3. 将物品放入缓冲区
    buffer.buffer[buffer.in] = item;
    buffer.in = (buffer.in + 1) % BUFFER_SIZE;
    buffer.count++;

    // 4. 释放互斥锁
    sem_signal(&buffer.mutex); // V(mutex)

    // 5. 增加满槽位信号量
    sem_signal(&buffer.full);  // V(full)

    // 打印状态
    printf("[%.3f] [Producer-%d] Produced item #%d, buffer count: %d\n",
           get_time(), item.producer_id, item.id, buffer.count);
}

// 消费函数
// 返回值：-1 表示应该退出（缓冲区为空且所有生产者已完成），0 表示成功消费
int consume(int consumer_id, Item *item) {
    // 1. 等待满槽位
    sem_wait(&buffer.full);    // P(full)

    // 2. 获取互斥锁
    sem_wait(&buffer.mutex);   // P(mutex)

    // 3. 检查是否应该退出（被唤醒后可能缓冲区为空且所有生产者已完成）
    if (buffer.count == 0) {
        // 检查所有生产者是否已完成
        base_lock_acquire(&producers_finished_lock);
        int all_producers_done = (producers_finished >= NUM_PRODUCERS);
        base_lock_release(&producers_finished_lock);

        if (all_producers_done) {
            // 所有生产者已完成且缓冲区为空，应该退出
            sem_signal(&buffer.mutex); // V(mutex)
            // 不需要 sem_signal(&buffer.empty)，因为没有消费物品
            return -1; // 表示应该退出
        }
    }

    // 4. 从缓冲区取出物品
    *item = buffer.buffer[buffer.out];
    buffer.out = (buffer.out + 1) % BUFFER_SIZE;
    buffer.count--;

    // 5. 释放互斥锁
    sem_signal(&buffer.mutex); // V(mutex)

    // 6. 增加空槽位信号量
    sem_signal(&buffer.empty); // V(empty)

    // 打印状态
    printf("[%.3f] [Consumer-%d] Consumed item #%d (from Producer-%d), buffer count: %d\n",
           get_time(), consumer_id, item->id, item->producer_id, buffer.count);

    return 0; // 成功消费
}

// 生产者线程
void* producer(void *arg) {
    int id = *(int*)arg;

    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        // 创建物品
        Item item;
        base_lock_acquire(&item_id_lock);
        item.id = next_item_id++;
        base_lock_release(&item_id_lock);

        item.producer_id = id;
        item.timestamp = get_time();

        // 生产物品
        produce(item);

        // 更新统计
        base_lock_acquire(&stats.lock);
        stats.produced[id]++;
        stats.total_produced++;
        base_lock_release(&stats.lock);

        // 模拟生产时间（场景3：50-150ms）
        usleep(rand() % 100000 + 50000);
    }

    printf("[Producer-%d] Finished. Total produced: %ld\n",
           id, stats.produced[id]);
    
    // 标记生产者完成
    base_lock_acquire(&producers_finished_lock);
    producers_finished++;
    int all_done = (producers_finished >= NUM_PRODUCERS);
    base_lock_release(&producers_finished_lock);
    
    
    return NULL;
}

// 消费者线程
void* consumer(void *arg) {
    int id = *(int*)arg;
    int expected_total = NUM_PRODUCERS * ITEMS_PER_PRODUCER;

    while (1) {
        // 检查是否所有物品都已生产且被消费完
        base_lock_acquire(&stats.lock);
        int total_consumed_now = stats.total_consumed;
        int total_produced_now = stats.total_produced;
        base_lock_release(&stats.lock);

        // 如果所有物品都已消费完，退出
        if (total_consumed_now >= expected_total) {
            break;
        }

        // 检查是否所有生产者都已完成
        base_lock_acquire(&producers_finished_lock);
        int all_producers_done = (producers_finished >= NUM_PRODUCERS);
        base_lock_release(&producers_finished_lock);

        // 如果所有生产者已完成，检查是否可以退出
        if (all_producers_done) {
            base_lock_acquire(&stats.lock);
            int total_produced_check = stats.total_produced;
            int total_consumed_check = stats.total_consumed;
            base_lock_release(&stats.lock);
            
            // 如果所有物品都已生产完，且已生产的物品都已消费完，退出
            if (total_produced_check >= expected_total && 
                total_produced_check == total_consumed_check) {
                break;
            }
            
            // 如果所有生产者已完成，但缓冲区为空（full_sem == 0），直接退出
            // 这样可以避免进入 consume() 函数后阻塞
            int full_sem_value = sem_get_value(&buffer.full);
            if (full_sem_value == 0) {
                // 再次检查统计信息，确保所有物品都已消费完
                base_lock_acquire(&stats.lock);
                int final_produced = stats.total_produced;
                int final_consumed = stats.total_consumed;
                base_lock_release(&stats.lock);
                
                if (final_produced >= expected_total && final_produced == final_consumed) {
                    break;
                }
            }
        }

        // 消费物品（只有在没有退出条件满足时才调用）
        Item item;
        int result = consume(id, &item);
        
        // 如果 consume() 返回 -1，表示应该退出
        if (result == -1) {
            break;
        }

        // 更新统计
        base_lock_acquire(&stats.lock);
        stats.consumed[id]++;
        stats.total_consumed++;
        int total_consumed_after = stats.total_consumed;
        int total_produced_after = stats.total_produced;
        base_lock_release(&stats.lock);
        
        // 消费后再次检查退出条件（防止在 consume() 中阻塞时状态发生变化）
        if (total_consumed_after >= expected_total) {
            break;
        }
        
        // 检查是否所有生产者已完成且所有物品都已消费完
        base_lock_acquire(&producers_finished_lock);
        int all_producers_done_after = (producers_finished >= NUM_PRODUCERS);
        base_lock_release(&producers_finished_lock);
        
        if (all_producers_done_after && total_produced_after >= expected_total && 
            total_produced_after == total_consumed_after) {
            break;
        }

        // 模拟消费时间（场景3：5-20ms）
        usleep(rand() % 15000 + 5000);
    }

    printf("[Consumer-%d] Finished. Total consumed: %ld\n",
           id, stats.consumed[id]);

    return NULL;
}

// 打印最终统计
void print_statistics() {
    printf("\n");
    printf("================== 统计信息  ==================\n");
    printf("生产者统计:\n");
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        printf("  Producer-%d: %ld items\n", i, stats.produced[i]);
    }
    printf("  总计 : %ld items\n", stats.total_produced);

    printf("\n消费者统计:\n");
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        printf("  Consumer-%d: %ld items\n", i, stats.consumed[i]);
    }
    printf("  总计 : %ld items\n", stats.total_consumed);

    printf("\n验证结果:\n");
    if (stats.total_produced == stats.total_consumed &&
        stats.total_produced == NUM_PRODUCERS * ITEMS_PER_PRODUCER) {
        printf("  ✓ 生产和消费数量匹配\n");
        printf("  ✓ 没有物品丢失\n");
    } else {
        printf("  ✗ 错误：生产=%ld, 消费=%ld, 期望=%d\n",
               stats.total_produced, stats.total_consumed,
               NUM_PRODUCERS * ITEMS_PER_PRODUCER);
    }

    printf("\n信号量状态:\n");
    sem_print_status(&buffer.empty);
    sem_print_status(&buffer.full);
    sem_print_status(&buffer.mutex);

    printf("==============================================\n");
}

int main() {
    srand(time(NULL));

    printf("=== 生产者-消费者问题模拟 ===\n");
    printf("配置 : %d个生产者 , %d个消费者 , 缓冲区大小=%d\n",
           NUM_PRODUCERS, NUM_CONSUMERS, BUFFER_SIZE);
    printf("每个生产者生产 %d 个物品\n\n", ITEMS_PER_PRODUCER);

    // 初始化
    buffer_init();
    base_lock_init(&stats.lock, "stats_lock");
    base_lock_init(&item_id_lock, "item_id_lock");
    base_lock_init(&producers_finished_lock, "producers_finished_lock");
    producers_finished = 0;

    // 创建线程
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    int producer_ids[NUM_PRODUCERS];
    int consumer_ids[NUM_CONSUMERS];

    double start_time = get_time();

    // 启动消费者
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // 启动生产者
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // 等待生产者结束
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    printf("\n[主线程] 所有生产者已完成\n");

    // 所有生产者完成后，唤醒所有等待的消费者
    // 这样可以让阻塞在 sem_wait(&buffer.full) 的消费者有机会检查退出条件
    // 注意：这会导致 full_sem 的值可能变为正数，但消费者会在检查后正确退出
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        sem_signal(&buffer.full);
    }

    printf("[主线程] 已唤醒所有消费者，等待消费者退出...\n");

    // 等待消费者结束
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    double end_time = get_time();
    printf("\n总运行时间 : %.2f 秒\n", end_time - start_time);

    print_statistics();

    // 清理：销毁信号量和锁
    sem_destroy(&buffer.empty);
    sem_destroy(&buffer.full);
    sem_destroy(&buffer.mutex);
    base_lock_destroy(&stats.lock);
    base_lock_destroy(&item_id_lock);
    base_lock_destroy(&producers_finished_lock);

    return 0;
}


