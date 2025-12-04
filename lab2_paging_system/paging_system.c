#include "paging_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 初始化分页系统
void init_paging_system(paging_system_t *ps) {
    // 初始化页表
    for (int i = 0; i < MAX_PAGES; i++) {
        ps->page_table[i].frame_num = -1;
        ps->page_table[i].valid = false;
        ps->page_table[i].modified = false;
        ps->page_table[i].referenced = false;
        ps->page_table[i].load_time = -1;
    }
    
    // 初始化物理帧
    for (int i = 0; i < MAX_FRAMES; i++) {
        ps->frames[i].page_num = -1;
        ps->frames[i].is_free = true;
        ps->frames[i].load_time = -1;
        memset(ps->physical_memory[i], 0, PAGE_SIZE);
    }
    
    // 初始化统计信息
    ps->page_fault_count = 0;
    ps->memory_access_count = 0;
    ps->current_time = 0;
    
    printf("Paging System Initialized\n");
    printf("Max Pages: %d\n", MAX_PAGES);
    printf("Physical Frames: %d\n", MAX_FRAMES);
    printf("Page Size: %d bytes\n", PAGE_SIZE);
    printf("Total Physical Memory: %d bytes\n\n", MEMORY_SIZE);
}

// 销毁分页系统
void destroy_paging_system(paging_system_t *ps) {
    // 清理资源
    memset(ps, 0, sizeof(paging_system_t));
}

// 查找空闲帧
int find_free_frame(paging_system_t *ps) {
    for (int i = 0; i < MAX_FRAMES; i++) {
        if (ps->frames[i].is_free) {
            return i;
        }
    }
    return -1;  // 没有空闲帧
}

// FIFO页面置换算法
int fifo_replace(paging_system_t *ps) {
    int oldest_frame = 0;
    int oldest_time = ps->frames[0].load_time;
    
    // 找到加载时间最早的帧
    for (int i = 1; i < MAX_FRAMES; i++) {
        if (ps->frames[i].load_time < oldest_time) {
            oldest_time = ps->frames[i].load_time;
            oldest_frame = i;
        }
    }
    
    printf("  [FIFO] Selected frame %d for replacement (load_time=%d)\n", 
           oldest_frame, oldest_time);
    
    return oldest_frame;
}

// 从磁盘加载页面
void load_from_disk(paging_system_t *ps, int page_num, int frame_num) {
    // 模拟从磁盘加载数据
    // 在实际系统中，这里会从磁盘读取数据到物理内存
    printf("  [DISK] Loading page %d into frame %d\n", page_num, frame_num);
    
    // 模拟数据：填充一些标识数据
    for (int i = 0; i < PAGE_SIZE; i++) {
        ps->physical_memory[frame_num][i] = (char)(page_num & 0xFF);
    }
}

// 保存到磁盘
void save_to_disk(paging_system_t *ps, int page_num, int frame_num) {
    // 模拟写回磁盘
    printf("  [DISK] Saving modified page %d (frame %d) to disk\n", 
           page_num, frame_num);
}

// 更新页表
void update_page_table(paging_system_t *ps, int page_num, int frame_num) {
    ps->page_table[page_num].frame_num = frame_num;
    ps->page_table[page_num].valid = true;
    ps->page_table[page_num].modified = false;
    ps->page_table[page_num].referenced = true;
    ps->page_table[page_num].load_time = ps->current_time;
    
    ps->frames[frame_num].page_num = page_num;
    ps->frames[frame_num].is_free = false;
    ps->frames[frame_num].load_time = ps->current_time;
}

// 缺页处理
void handle_page_fault(paging_system_t *ps, int page_num) {
    printf("  [PAGE FAULT] Page %d not in memory\n", page_num);
    ps->page_fault_count++;
    
    // 查找空闲帧
    int frame_num = find_free_frame(ps);
    
    if (frame_num == -1) {
        // 没有空闲帧，需要页面置换
        printf("  [REPLACEMENT] No free frame available, need replacement\n");
        frame_num = fifo_replace(ps);
        
        // 获取被置换页面的页号
        int victim_page = ps->frames[frame_num].page_num;
        
        // 如果被置换页面已修改，写回磁盘
        if (ps->page_table[victim_page].modified) {
            save_to_disk(ps, victim_page, frame_num);
        }
        
        // 使被置换页面的页表项无效
        ps->page_table[victim_page].valid = false;
        ps->page_table[victim_page].frame_num = -1;
        
        printf("  [REPLACEMENT] Replaced page %d from frame %d\n", 
               victim_page, frame_num);
    } else {
        printf("  [ALLOCATION] Found free frame %d\n", frame_num);
    }
    
    // 从磁盘加载页面
    load_from_disk(ps, page_num, frame_num);
    
    // 更新页表
    update_page_table(ps, page_num, frame_num);
    
    printf("  [SUCCESS] Page %d loaded into frame %d\n", page_num, frame_num);
}

// 地址转换
int translate_address(paging_system_t *ps, int logical_addr, char operation) {
    ps->memory_access_count++;
    ps->current_time++;
    
    // 计算页号和页内偏移
    int page_num = logical_addr / PAGE_SIZE;
    int offset = logical_addr % PAGE_SIZE;
    
    printf("\n[ACCESS %d] Logical Address: %d (Page: %d, Offset: %d) Operation: %c\n",
           ps->memory_access_count, logical_addr, page_num, offset, operation);
    
    // 检查页号是否合法
    if (page_num < 0 || page_num >= MAX_PAGES) {
        printf("  [ERROR] Invalid page number: %d\n", page_num);
        return -1;
    }
    
    // 检查页面是否在内存中
    if (!ps->page_table[page_num].valid) {
        handle_page_fault(ps, page_num);
    } else {
        printf("  [HIT] Page %d is in frame %d\n", 
               page_num, ps->page_table[page_num].frame_num);
    }
    
    // 更新访问位
    ps->page_table[page_num].referenced = true;
    
    // 如果是写操作，设置修改位
    if (operation == OP_WRITE || operation == 'W') {
        ps->page_table[page_num].modified = true;
        printf("  [WRITE] Page %d marked as modified\n", page_num);
    }
    
    // 计算物理地址
    int frame_num = ps->page_table[page_num].frame_num;
    int physical_addr = frame_num * PAGE_SIZE + offset;
    
    printf("  [RESULT] Physical Address: %d (Frame: %d, Offset: %d)\n",
           physical_addr, frame_num, offset);
    
    return physical_addr;
}

// 打印页表
void print_page_table(paging_system_t *ps) {
    printf("\n========== Page Table ==========\n");
    printf("Page | Frame | Valid | Modified | Referenced | LoadTime\n");
    printf("-----+-------+-------+----------+------------+---------\n");
    
    for (int i = 0; i < MAX_PAGES; i++) {
        if (ps->page_table[i].valid) {
            printf("%4d | %5d | %5s | %8s | %10s | %8d\n",
                   i,
                   ps->page_table[i].frame_num,
                   ps->page_table[i].valid ? "Y" : "N",
                   ps->page_table[i].modified ? "Y" : "N",
                   ps->page_table[i].referenced ? "Y" : "N",
                   ps->page_table[i].load_time);
        }
    }
    printf("================================\n\n");
}

// 打印物理帧信息
void print_frame_info(paging_system_t *ps) {
    printf("\n========== Frame Information ==========\n");
    printf("Frame | Page | Status | LoadTime\n");
    printf("------+------+--------+---------\n");
    
    for (int i = 0; i < MAX_FRAMES; i++) {
        printf("%5d | %4d | %6s | %8d\n",
               i,
               ps->frames[i].page_num,
               ps->frames[i].is_free ? "FREE" : "USED",
               ps->frames[i].load_time);
    }
    printf("=======================================\n\n");
}

// 打印统计信息
void print_statistics(paging_system_t *ps) {
    printf("\n========== Statistics ==========\n");
    printf("Total Memory Accesses: %d\n", ps->memory_access_count);
    printf("Page Faults: %d\n", ps->page_fault_count);
    printf("Page Fault Rate: %.2f%%\n", get_page_fault_rate(ps));
    printf("Page Hits: %d\n", ps->memory_access_count - ps->page_fault_count);
    printf("Hit Rate: %.2f%%\n", 
           100.0 - get_page_fault_rate(ps));
    printf("================================\n\n");
}

// 获取缺页率
double get_page_fault_rate(paging_system_t *ps) {
    if (ps->memory_access_count == 0) {
        return 0.0;
    }
    return (double)ps->page_fault_count / ps->memory_access_count * 100.0;
}

