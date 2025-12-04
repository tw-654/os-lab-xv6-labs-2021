#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 初始化内存管理器
void init_memory_manager(memory_manager_t *mm, int total_size) {
    mm->total_size = total_size;
    mm->allocated_count = 0;
    mm->released_count = 0;
    
    // 创建初始的空闲分区（整个内存都是空闲的）
    mm->free_list = (free_area_t *)malloc(sizeof(free_area_t));
    if (mm->free_list == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for free list\n");
        exit(1);
    }
    mm->free_list->start_addr = 0;
    mm->free_list->size = total_size;
    mm->free_list->next = NULL;
}

// 销毁内存管理器
void destroy_memory_manager(memory_manager_t *mm) {
    free_area_t *current = mm->free_list;
    while (current != NULL) {
        free_area_t *next = current->next;
        free(current);
        current = next;
    }
    mm->free_list = NULL;
}

// 首次适应算法(First Fit)
int first_fit_allocate(memory_manager_t *mm, int size) {
    free_area_t *prev = NULL;
    free_area_t *current = mm->free_list;
    
    // 从内存起始地址开始查找第一个能满足要求的空闲分区
    while (current != NULL) {
        if (current->size >= size) {
            // 找到合适的分区
            int allocated_addr = current->start_addr;
            
            if (current->size == size) {
                // 分区大小正好，移除该节点
                if (prev == NULL) {
                    mm->free_list = current->next;
                } else {
                    prev->next = current->next;
                }
                free(current);
            } else {
                // 分区大小大于需求，更新分区
                current->start_addr += size;
                current->size -= size;
            }
            
            mm->allocated_count++;
            return allocated_addr;
        }
        prev = current;
        current = current->next;
    }
    
    // 没有找到合适的空闲分区
    return -1;
}

// 最佳适应算法(Best Fit)
int best_fit_allocate(memory_manager_t *mm, int size) {
    free_area_t *best_prev = NULL;
    free_area_t *best_current = NULL;
    free_area_t *prev = NULL;
    free_area_t *current = mm->free_list;
    int best_size = mm->total_size + 1; // 初始化为一个不可能的大值
    
    // 遍历空闲链表，找到满足条件的最小分区
    while (current != NULL) {
        if (current->size >= size && current->size < best_size) {
            best_size = current->size;
            best_current = current;
            best_prev = prev;
        }
        prev = current;
        current = current->next;
    }
    
    // 如果找到了合适的分区
    if (best_current != NULL) {
        int allocated_addr = best_current->start_addr;
        
        if (best_current->size == size) {
            // 分区大小正好，移除该节点
            if (best_prev == NULL) {
                mm->free_list = best_current->next;
            } else {
                best_prev->next = best_current->next;
            }
            free(best_current);
        } else {
            // 分区大小大于需求，更新分区
            best_current->start_addr += size;
            best_current->size -= size;
        }
        
        mm->allocated_count++;
        return allocated_addr;
    }
    
    // 没有找到合适的空闲分区
    return -1;
}

// 最坏适应算法(Worst Fit)
int worst_fit_allocate(memory_manager_t *mm, int size) {
    free_area_t *worst_prev = NULL;
    free_area_t *worst_current = NULL;
    free_area_t *prev = NULL;
    free_area_t *current = mm->free_list;
    int worst_size = -1; // 初始化为-1
    
    // 遍历空闲链表，找到满足条件的最大分区
    while (current != NULL) {
        if (current->size >= size && current->size > worst_size) {
            worst_size = current->size;
            worst_current = current;
            worst_prev = prev;
        }
        prev = current;
        current = current->next;
    }
    
    // 如果找到了合适的分区
    if (worst_current != NULL) {
        int allocated_addr = worst_current->start_addr;
        
        if (worst_current->size == size) {
            // 分区大小正好，移除该节点
            if (worst_prev == NULL) {
                mm->free_list = worst_current->next;
            } else {
                worst_prev->next = worst_current->next;
            }
            free(worst_current);
        } else {
            // 分区大小大于需求，更新分区
            worst_current->start_addr += size;
            worst_current->size -= size;
        }
        
        mm->allocated_count++;
        return allocated_addr;
    }
    
    // 没有找到合适的空闲分区
    return -1;
}

// 内存释放和分区合并
bool release_memory(memory_manager_t *mm, int addr, int size) {
    // 创建新的空闲分区
    free_area_t *new_area = (free_area_t *)malloc(sizeof(free_area_t));
    if (new_area == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for new free area\n");
        return false;
    }
    new_area->start_addr = addr;
    new_area->size = size;
    new_area->next = NULL;
    
    // 找到合适的插入位置（按地址排序）
    free_area_t *prev = NULL;
    free_area_t *current = mm->free_list;
    
    while (current != NULL && current->start_addr < addr) {
        prev = current;
        current = current->next;
    }
    
    // 插入新分区
    if (prev == NULL) {
        new_area->next = mm->free_list;
        mm->free_list = new_area;
    } else {
        new_area->next = prev->next;
        prev->next = new_area;
    }
    
    // 合并相邻的空闲分区
    // 检查是否可以与前一个分区合并
    if (prev != NULL && prev->start_addr + prev->size == new_area->start_addr) {
        prev->size += new_area->size;
        prev->next = new_area->next;
        free(new_area);
        new_area = prev;
    }
    
    // 检查是否可以与后一个分区合并
    if (new_area->next != NULL && 
        new_area->start_addr + new_area->size == new_area->next->start_addr) {
        new_area->size += new_area->next->size;
        free_area_t *to_free = new_area->next;
        new_area->next = to_free->next;
        free(to_free);
    }
    
    mm->released_count++;
    return true;
}

// 打印内存状态
void print_memory_status(memory_manager_t *mm) {
    printf("=== Memory Status ===\n");
    printf("Total Memory: %d KB\n", mm->total_size);
    printf("Allocated Count: %d\n", mm->allocated_count);
    printf("Released Count: %d\n", mm->released_count);
    printf("Free Memory: %d KB\n", get_free_memory(mm));
    printf("Fragmentation: %d KB\n", calculate_fragmentation(mm));
    
    printf("\nFree Areas:\n");
    free_area_t *current = mm->free_list;
    int area_num = 1;
    while (current != NULL) {
        printf("  Area %d: [%d, %d) size=%d KB\n", 
               area_num++, current->start_addr, 
               current->start_addr + current->size, current->size);
        current = current->next;
    }
    printf("\n");
}

// 计算内存碎片（所有空闲分区中，除了最大分区之外的总大小）
int calculate_fragmentation(memory_manager_t *mm) {
    if (mm->free_list == NULL) {
        return 0;
    }
    
    int max_size = 0;
    int total_free = 0;
    free_area_t *current = mm->free_list;
    
    while (current != NULL) {
        total_free += current->size;
        if (current->size > max_size) {
            max_size = current->size;
        }
        current = current->next;
    }
    
    // 碎片 = 总空闲内存 - 最大空闲分区
    return total_free - max_size;
}

// 获取总空闲内存
int get_free_memory(memory_manager_t *mm) {
    int total_free = 0;
    free_area_t *current = mm->free_list;
    
    while (current != NULL) {
        total_free += current->size;
        current = current->next;
    }
    
    return total_free;
}

