#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdbool.h>

// 空闲分区结构
typedef struct free_area {
    int start_addr;          // 起始地址
    int size;                // 分区大小
    struct free_area *next;   // 下一个分区指针
} free_area_t;

// 内存管理器
typedef struct {
    free_area_t *free_list;   // 空闲分区链表
    int total_size;          // 总内存大小
    int allocated_count;      // 分配次数统计
    int released_count;       // 释放次数统计
} memory_manager_t;

// 已分配内存块结构（用于跟踪分配的内存）
typedef struct allocated_block {
    int start_addr;
    int size;
    struct allocated_block *next;
} allocated_block_t;

// 函数声明
void init_memory_manager(memory_manager_t *mm, int total_size);
void destroy_memory_manager(memory_manager_t *mm);

// 三种分配算法
int first_fit_allocate(memory_manager_t *mm, int size);
int best_fit_allocate(memory_manager_t *mm, int size);
int worst_fit_allocate(memory_manager_t *mm, int size);

// 内存释放
bool release_memory(memory_manager_t *mm, int addr, int size);

// 辅助函数
void print_memory_status(memory_manager_t *mm);
int calculate_fragmentation(memory_manager_t *mm);
int get_free_memory(memory_manager_t *mm);

#endif // MEMORY_MANAGER_H

