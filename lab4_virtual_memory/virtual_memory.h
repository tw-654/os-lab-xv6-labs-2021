#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include <stdbool.h>

#define PAGE_SIZE 4096           // 页面大小（4KB）
#define MAX_PAGES 256            // 最大逻辑页面数
#define MAX_PHYSICAL_FRAMES 4    // 物理内存帧数
#define MAX_DISK_PAGES 256       // 磁盘存储页面数

// 页表项结构
typedef struct {
    int frame_num;              // 物理帧号（-1表示未分配）
    bool valid;                 // 有效位（是否在物理内存中）
    bool modified;              // 修改位（脏位）
    bool referenced;            // 访问位（引用位）
    int last_access_time;       // 最后访问时间（用于LRU）
    int load_time;              // 页面加载时间
} pte_t;

// 虚拟内存系统结构
typedef struct {
    pte_t page_table[MAX_PAGES];                    // 页表（256个条目）
    char physical_memory[MAX_PHYSICAL_FRAMES][PAGE_SIZE];  // 物理内存（4帧，每帧4KB）
    char disk_storage[MAX_DISK_PAGES][PAGE_SIZE];   // 磁盘存储（256页，每页4KB）
    int page_faults;                                // 缺页统计
    int disk_io_count;                              // 磁盘 I/O 统计
    int current_time;                               // 当前时间戳（用于LRU）
    int frame_page_map[MAX_PHYSICAL_FRAMES];        // 帧到页的映射（-1表示空闲）
} virtual_memory_system_t;

// 函数声明

// 初始化虚拟内存系统
void init_virtual_memory_system(virtual_memory_system_t *vms);

// 任务 5.3.1：实现完整地址转换
int vm_translate(virtual_memory_system_t *vms, int logical_addr, char operation);

// 任务 5.3.2：实现 LRU 置换算法
int lru_replace(virtual_memory_system_t *vms);

// 辅助函数
int find_free_frame(virtual_memory_system_t *vms);
void handle_page_fault(virtual_memory_system_t *vms, int page_num);
void load_page_from_disk(virtual_memory_system_t *vms, int page_num, int frame_num);
void write_page_to_disk(virtual_memory_system_t *vms, int page_num, int frame_num);
void update_access_info(virtual_memory_system_t *vms, int page_num);

// 统计和显示
void print_page_table(virtual_memory_system_t *vms, int start_page, int end_page);
void print_physical_memory_status(virtual_memory_system_t *vms);
void print_statistics(virtual_memory_system_t *vms);
double get_page_fault_rate(virtual_memory_system_t *vms, int total_accesses);

#endif // VIRTUAL_MEMORY_H

