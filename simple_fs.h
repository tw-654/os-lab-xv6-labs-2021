#ifndef SIMPLE_FS_H
#define SIMPLE_FS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========== 基础常量定义 ==========
#define DISK_SIZE  (1024 * 1024)    // 磁盘大小1MB
#define BLOCK_SIZE 512              // 块大小512字节
#define BLOCK_NUM  (DISK_SIZE / BLOCK_SIZE)  // 总块数 = 2048
#define MAX_FILES  32               // 最大文件数（极大简化，固定数组）
#define MAX_FILENAME_LEN 20

// ========== 数据结构定义 ==========

// 文件控制块 (FCB)
typedef struct {
    char name[MAX_FILENAME_LEN]; // 文件名
    int size;                    // 文件大小（字节）
    int start_block;             // 起始块号（连续分配）
    int is_valid;                // 该FCB是否有效（是否被使用）
} fcb_t;

// 简易文件系统管理器
typedef struct {
    fcb_t fcb_table[MAX_FILES];      // FCB表，固定大小，模拟所有文件
    unsigned char disk[DISK_SIZE];   // 模拟磁盘空间（内存数组）
    int bit_map[BLOCK_NUM];          // 位示图，用于空闲块管理（简化为int数组，0=空闲，1=占用）
} simple_fs_t;

// ========== 函数声明 ==========

// 初始化文件系统
void init_fs(simple_fs_t* fs);

// 任务1：查找连续的空闲块
int find_free_blocks(simple_fs_t* fs, int block_count);

// 任务2：创建文件
int my_create(simple_fs_t* fs, const char* filename, int size);

// 任务3：写入文件
int my_write(simple_fs_t* fs, int fd, const char* data);

// 任务4：读取文件
int my_read(simple_fs_t* fs, int fd, char* buffer, int buffer_size);

// 扩展任务1：删除文件
int my_delete(simple_fs_t* fs, int fd);

// 扩展任务2：显示磁盘状态
void show_disk_status(simple_fs_t* fs);

// 辅助函数：列出所有文件
void list_files(simple_fs_t* fs);

#endif // SIMPLE_FS_H

