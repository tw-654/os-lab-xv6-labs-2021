#ifndef PAGING_SYSTEM_H
#define PAGING_SYSTEM_H

#include <stdbool.h>

#define MAX_PAGES 256       // 最大页面数
#define MAX_FRAMES 4        // 物理内存帧数
#define PAGE_SIZE 4096      // 页面大小（4KB）
#define MEMORY_SIZE (MAX_FRAMES * PAGE_SIZE)  // 物理内存大小

// 页表项结构
typedef struct page_table_entry {
    int frame_num;          // 物理帧号（-1表示未分配）
    bool valid;             // 有效位（是否在物理内存中）
    bool modified;          // 修改位（脏位）
    bool referenced;        // 访问位（引用位）
    int load_time;          // 页面加载时间（用于FIFO）
} pte_t;

// 物理帧结构
typedef struct frame_info {
    int page_num;           // 存储的页面号（-1表示空闲）
    bool is_free;           // 是否空闲
    int load_time;          // 加载时间戳
} frame_info_t;

// 分页系统管理器
typedef struct {
    pte_t page_table[MAX_PAGES];         // 页表
    frame_info_t frames[MAX_FRAMES];     // 物理帧信息
    char physical_memory[MAX_FRAMES][PAGE_SIZE]; // 物理内存
    int page_fault_count;                // 缺页次数统计
    int memory_access_count;             // 内存访问次数统计
    int current_time;                    // 当前时间戳
} paging_system_t;

// 访问操作类型
typedef enum {
    OP_READ = 'R',
    OP_WRITE = 'W'
} operation_t;

// 函数声明

// 初始化和销毁
void init_paging_system(paging_system_t *ps);
void destroy_paging_system(paging_system_t *ps);

// 地址转换
int translate_address(paging_system_t *ps, int logical_addr, char operation);

// 缺页处理
void handle_page_fault(paging_system_t *ps, int page_num);

// 页面置换算法
int fifo_replace(paging_system_t *ps);

// 辅助函数
int find_free_frame(paging_system_t *ps);
void update_page_table(paging_system_t *ps, int page_num, int frame_num);
void load_from_disk(paging_system_t *ps, int page_num, int frame_num);
void save_to_disk(paging_system_t *ps, int page_num, int frame_num);

// 统计和显示
void print_page_table(paging_system_t *ps);
void print_frame_info(paging_system_t *ps);
void print_statistics(paging_system_t *ps);
double get_page_fault_rate(paging_system_t *ps);

#endif // PAGING_SYSTEM_H

