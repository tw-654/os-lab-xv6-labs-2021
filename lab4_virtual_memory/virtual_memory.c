#include "virtual_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// 初始化虚拟内存系统
void init_virtual_memory_system(virtual_memory_system_t *vms) {
    // 初始化页表
    for (int i = 0; i < MAX_PAGES; i++) {
        vms->page_table[i].frame_num = -1;
        vms->page_table[i].valid = false;
        vms->page_table[i].modified = false;
        vms->page_table[i].referenced = false;
        vms->page_table[i].last_access_time = -1;
        vms->page_table[i].load_time = -1;
    }
    
    // 初始化物理内存
    for (int i = 0; i < MAX_PHYSICAL_FRAMES; i++) {
        memset(vms->physical_memory[i], 0, PAGE_SIZE);
        vms->frame_page_map[i] = -1;  // 标记为空闲
    }
    
    // 初始化磁盘存储（模拟已有数据）
    for (int i = 0; i < MAX_DISK_PAGES; i++) {
        // 模拟磁盘数据：每个页面填充页面号作为标识
        for (int j = 0; j < PAGE_SIZE; j++) {
            vms->disk_storage[i][j] = (char)(i & 0xFF);
        }
    }
    
    // 初始化统计信息
    vms->page_faults = 0;
    vms->disk_io_count = 0;
    vms->current_time = 0;
    
    printf("Virtual Memory System Initialized\n");
    printf("  Page Table: %d entries\n", MAX_PAGES);
    printf("  Physical Memory: %d frames x %d KB = %d KB\n", 
           MAX_PHYSICAL_FRAMES, PAGE_SIZE / 1024, 
           MAX_PHYSICAL_FRAMES * PAGE_SIZE / 1024);
    printf("  Disk Storage: %d pages x %d KB = %d KB\n\n", 
           MAX_DISK_PAGES, PAGE_SIZE / 1024, 
           MAX_DISK_PAGES * PAGE_SIZE / 1024);
}

// 查找空闲物理帧
int find_free_frame(virtual_memory_system_t *vms) {
    for (int i = 0; i < MAX_PHYSICAL_FRAMES; i++) {
        if (vms->frame_page_map[i] == -1) {
            return i;
        }
    }
    return -1;  // 没有空闲帧
}

// 从磁盘加载页面到物理内存
void load_page_from_disk(virtual_memory_system_t *vms, int page_num, int frame_num) {
    // 从磁盘复制数据到物理内存
    memcpy(vms->physical_memory[frame_num], 
           vms->disk_storage[page_num], 
           PAGE_SIZE);
    
    vms->disk_io_count++;  // 增加磁盘I/O计数
    
    // 更新帧到页的映射
    vms->frame_page_map[frame_num] = page_num;
}

// 将页面写回磁盘
void write_page_to_disk(virtual_memory_system_t *vms, int page_num, int frame_num) {
    // 从物理内存复制数据到磁盘
    memcpy(vms->disk_storage[page_num], 
           vms->physical_memory[frame_num], 
           PAGE_SIZE);
    
    vms->disk_io_count++;  // 增加磁盘I/O计数
}

// 更新页面访问信息
void update_access_info(virtual_memory_system_t *vms, int page_num) {
    if (page_num >= 0 && page_num < MAX_PAGES) {
        vms->page_table[page_num].referenced = true;
        vms->page_table[page_num].last_access_time = vms->current_time;
    }
}

// 任务 5.3.2：实现 LRU 置换算法
int lru_replace(virtual_memory_system_t *vms) {
    int victim_frame = -1;
    int oldest_access_time = INT_MAX;
    
    // 遍历所有物理帧，找到最久未使用的页面
    for (int i = 0; i < MAX_PHYSICAL_FRAMES; i++) {
        int page_num = vms->frame_page_map[i];
        
        if (page_num != -1) {  // 帧被占用
            int last_access = vms->page_table[page_num].last_access_time;
            
            // 找到最久未访问的页面
            if (last_access < oldest_access_time) {
                oldest_access_time = last_access;
                victim_frame = i;
            }
        }
    }
    
    if (victim_frame != -1) {
        int victim_page = vms->frame_page_map[victim_frame];
        
        // 处理页面写回：如果页面被修改过，需要写回磁盘
        if (vms->page_table[victim_page].modified) {
            write_page_to_disk(vms, victim_page, victim_frame);
        }
        
        // 更新页表：标记页面不在物理内存中
        vms->page_table[victim_page].valid = false;
        vms->page_table[victim_page].frame_num = -1;
        
        // 清空帧映射
        vms->frame_page_map[victim_frame] = -1;
    }
    
    return victim_frame;
}

// 处理缺页中断
void handle_page_fault(virtual_memory_system_t *vms, int page_num) {
    vms->page_faults++;  // 增加缺页计数
    
    // 查找空闲物理帧
    int free_frame = find_free_frame(vms);
    
    if (free_frame == -1) {
        // 没有空闲帧，需要进行页面置换
        free_frame = lru_replace(vms);
    }
    
    if (free_frame != -1) {
        // 从磁盘加载页面
        load_page_from_disk(vms, page_num, free_frame);
        
        // 更新页表
        vms->page_table[page_num].frame_num = free_frame;
        vms->page_table[page_num].valid = true;
        vms->page_table[page_num].modified = false;
        vms->page_table[page_num].referenced = true;
        vms->page_table[page_num].last_access_time = vms->current_time;
        vms->page_table[page_num].load_time = vms->current_time;
    }
}

// 任务 5.3.1：实现完整地址转换
int vm_translate(virtual_memory_system_t *vms, int logical_addr, char operation) {
    // 1. 计算页号和偏移量
    int page_num = logical_addr / PAGE_SIZE;
    int offset = logical_addr % PAGE_SIZE;
    
    // 检查逻辑地址有效性
    if (page_num < 0 || page_num >= MAX_PAGES) {
        printf("  [ERROR] Invalid page number: %d (logical_addr=%d)\n", 
               page_num, logical_addr);
        return -1;
    }
    
    if (offset < 0 || offset >= PAGE_SIZE) {
        printf("  [ERROR] Invalid offset: %d\n", offset);
        return -1;
    }
    
    // 2. 检查页表有效性
    vms->current_time++;  // 更新时间戳
    
    if (!vms->page_table[page_num].valid) {
        // 3. 处理缺页中断
        handle_page_fault(vms, page_num);
    }
    
    // 4. 更新访问信息
    update_access_info(vms, page_num);
    
    // 如果是写操作，设置修改位
    if (operation == 'W' || operation == 'w') {
        vms->page_table[page_num].modified = true;
    }
    
    // 5. 返回物理地址 = 帧号 * 页面大小 + 偏移量
    int frame_num = vms->page_table[page_num].frame_num;
    if (frame_num < 0 || frame_num >= MAX_PHYSICAL_FRAMES) {
        printf("  [ERROR] Invalid frame number: %d\n", frame_num);
        return -1;
    }
    
    int physical_addr = frame_num * PAGE_SIZE + offset;
    return physical_addr;
}

// 打印页表信息
void print_page_table(virtual_memory_system_t *vms, int start_page, int end_page) {
    if (start_page < 0) start_page = 0;
    if (end_page >= MAX_PAGES) end_page = MAX_PAGES - 1;
    
    printf("\nPage Table (pages %d-%d):\n", start_page, end_page);
    printf("Page | Frame | Valid | Modified | Referenced | Last Access | Load Time\n");
    printf("-----+-------+-------+----------+------------+-------------+----------\n");
    
    for (int i = start_page; i <= end_page; i++) {
        pte_t *pte = &vms->page_table[i];
        if (pte->valid || i < 20) {  // 只显示有效的页表项或前20项
            printf("%4d | %5d | %5s | %8s | %10s | %11d | %10d\n",
                   i,
                   pte->frame_num,
                   pte->valid ? "Yes" : "No",
                   pte->modified ? "Yes" : "No",
                   pte->referenced ? "Yes" : "No",
                   pte->last_access_time,
                   pte->load_time);
        }
    }
    printf("\n");
}

// 打印物理内存状态
void print_physical_memory_status(virtual_memory_system_t *vms) {
    printf("\nPhysical Memory Status:\n");
    printf("Frame | Page | Status\n");
    printf("------+------+--------\n");
    
    for (int i = 0; i < MAX_PHYSICAL_FRAMES; i++) {
        int page_num = vms->frame_page_map[i];
        printf("%5d | %4d | %s\n", 
               i, 
               page_num, 
               page_num == -1 ? "Free" : "Occupied");
    }
    printf("\n");
}

// 打印统计信息
void print_statistics(virtual_memory_system_t *vms) {
    printf("\n========================================\n");
    printf("Virtual Memory System Statistics\n");
    printf("========================================\n");
    printf("Page Faults:        %d\n", vms->page_faults);
    printf("Disk I/O Count:     %d\n", vms->disk_io_count);
    printf("Current Time:       %d\n", vms->current_time);
    printf("\n");
}

// 计算缺页率
double get_page_fault_rate(virtual_memory_system_t *vms, int total_accesses) {
    if (total_accesses == 0) {
        return 0.0;
    }
    return (double)vms->page_faults / total_accesses * 100.0;
}

