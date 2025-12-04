#include "page_replacement.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// 初始化页面置换管理器
void init_replacement_manager(replacement_manager_t *rm, int frame_count) {
    rm->frame_count = frame_count;
    rm->current_time = 0;
    rm->page_fault_count = 0;
    rm->page_hit_count = 0;
    rm->total_access_count = 0;
    
    for (int i = 0; i < frame_count; i++) {
        rm->frames[i].page_num = -1;
        rm->frames[i].load_time = -1;
        rm->frames[i].last_access_time = -1;
    }
}

// 重置管理器（保持帧数不变）
void reset_replacement_manager(replacement_manager_t *rm) {
    int frame_count = rm->frame_count;
    init_replacement_manager(rm, frame_count);
}

// 在帧中查找页面
int find_page_in_frames(replacement_manager_t *rm, int page_num) {
    for (int i = 0; i < rm->frame_count; i++) {
        if (rm->frames[i].page_num == page_num) {
            return i;
        }
    }
    return -1;  // 未找到
}

// 查找空闲帧
int find_free_frame(replacement_manager_t *rm) {
    for (int i = 0; i < rm->frame_count; i++) {
        if (rm->frames[i].page_num == -1) {
            return i;
        }
    }
    return -1;  // 没有空闲帧
}

// FIFO选择牺牲页面
int fifo_select_victim(replacement_manager_t *rm) {
    int victim = 0;
    int oldest_time = rm->frames[0].load_time;
    
    for (int i = 1; i < rm->frame_count; i++) {
        if (rm->frames[i].load_time < oldest_time) {
            oldest_time = rm->frames[i].load_time;
            victim = i;
        }
    }
    
    return victim;
}

// LRU选择牺牲页面
int lru_select_victim(replacement_manager_t *rm) {
    int victim = 0;
    int oldest_access = rm->frames[0].last_access_time;
    
    for (int i = 1; i < rm->frame_count; i++) {
        if (rm->frames[i].last_access_time < oldest_access) {
            oldest_access = rm->frames[i].last_access_time;
            victim = i;
        }
    }
    
    return victim;
}

// OPTIMAL选择牺牲页面
int optimal_select_victim(replacement_manager_t *rm, int *sequence, 
                          int current_pos, int length) {
    int victim = 0;
    int farthest = -1;
    
    // 对每个帧中的页面，找到它在未来的下一次使用位置
    for (int i = 0; i < rm->frame_count; i++) {
        int page = rm->frames[i].page_num;
        int next_use = INT_MAX;  // 假设永远不再使用
        
        // 在未来的访问序列中查找这个页面
        for (int j = current_pos + 1; j < length; j++) {
            if (sequence[j] == page) {
                next_use = j;
                break;
            }
        }
        
        // 选择未来最晚使用（或不再使用）的页面
        if (next_use > farthest) {
            farthest = next_use;
            victim = i;
        }
    }
    
    return victim;
}

// 加载页面到帧
void load_page(replacement_manager_t *rm, int frame_index, int page_num) {
    rm->frames[frame_index].page_num = page_num;
    rm->frames[frame_index].load_time = rm->current_time;
    rm->frames[frame_index].last_access_time = rm->current_time;
}

// 访问页面（更新访问时间）
void access_page(replacement_manager_t *rm, int frame_index) {
    rm->frames[frame_index].last_access_time = rm->current_time;
}

// FIFO算法实现
int fifo_algorithm(replacement_manager_t *rm, int *sequence, int length) {
    reset_replacement_manager(rm);
    
    for (int i = 0; i < length; i++) {
        int page = sequence[i];
        rm->current_time++;
        rm->total_access_count++;
        
        // 检查页面是否在内存中
        int frame_index = find_page_in_frames(rm, page);
        
        if (frame_index != -1) {
            // 页面命中
            rm->page_hit_count++;
        } else {
            // 缺页
            rm->page_fault_count++;
            
            // 查找空闲帧
            frame_index = find_free_frame(rm);
            
            if (frame_index == -1) {
                // 没有空闲帧，需要置换
                frame_index = fifo_select_victim(rm);
            }
            
            // 加载新页面
            load_page(rm, frame_index, page);
        }
    }
    
    return rm->page_fault_count;
}

// LRU算法实现
int lru_algorithm(replacement_manager_t *rm, int *sequence, int length) {
    reset_replacement_manager(rm);
    
    for (int i = 0; i < length; i++) {
        int page = sequence[i];
        rm->current_time++;
        rm->total_access_count++;
        
        // 检查页面是否在内存中
        int frame_index = find_page_in_frames(rm, page);
        
        if (frame_index != -1) {
            // 页面命中，更新访问时间
            rm->page_hit_count++;
            access_page(rm, frame_index);
        } else {
            // 缺页
            rm->page_fault_count++;
            
            // 查找空闲帧
            frame_index = find_free_frame(rm);
            
            if (frame_index == -1) {
                // 没有空闲帧，使用LRU选择牺牲页面
                frame_index = lru_select_victim(rm);
            }
            
            // 加载新页面
            load_page(rm, frame_index, page);
        }
    }
    
    return rm->page_fault_count;
}

// OPTIMAL算法实现
int optimal_algorithm(replacement_manager_t *rm, int *sequence, int length) {
    reset_replacement_manager(rm);
    
    for (int i = 0; i < length; i++) {
        int page = sequence[i];
        rm->current_time++;
        rm->total_access_count++;
        
        // 检查页面是否在内存中
        int frame_index = find_page_in_frames(rm, page);
        
        if (frame_index != -1) {
            // 页面命中
            rm->page_hit_count++;
        } else {
            // 缺页
            rm->page_fault_count++;
            
            // 查找空闲帧
            frame_index = find_free_frame(rm);
            
            if (frame_index == -1) {
                // 没有空闲帧，使用OPTIMAL选择牺牲页面
                frame_index = optimal_select_victim(rm, sequence, i, length);
            }
            
            // 加载新页面
            load_page(rm, frame_index, page);
        }
    }
    
    return rm->page_fault_count;
}

// 打印帧状态
void print_frame_status(replacement_manager_t *rm) {
    printf("Frames: [");
    for (int i = 0; i < rm->frame_count; i++) {
        if (rm->frames[i].page_num == -1) {
            printf(" - ");
        } else {
            printf(" %d ", rm->frames[i].page_num);
        }
        if (i < rm->frame_count - 1) {
            printf("|");
        }
    }
    printf("]\n");
}

// 打印统计信息
void print_statistics(replacement_manager_t *rm) {
    printf("\n========== Statistics ==========\n");
    printf("Total Accesses: %d\n", rm->total_access_count);
    printf("Page Faults: %d\n", rm->page_fault_count);
    printf("Page Hits: %d\n", rm->page_hit_count);
    printf("Page Fault Rate: %.2f%%\n", get_page_fault_rate(rm));
    printf("Hit Rate: %.2f%%\n", 100.0 - get_page_fault_rate(rm));
    printf("================================\n\n");
}

// 获取缺页率
double get_page_fault_rate(replacement_manager_t *rm) {
    if (rm->total_access_count == 0) {
        return 0.0;
    }
    return (double)rm->page_fault_count / rm->total_access_count * 100.0;
}

// 打印测试结果
void print_test_result(test_result_t *result) {
    printf("%-12s | Frames: %d | Faults: %2d | Hits: %2d | Fault Rate: %5.2f%%\n",
           result->algorithm_name,
           result->frame_count,
           result->page_fault_count,
           result->page_hit_count,
           result->page_fault_rate);
}

// 打印对比表格
void print_comparison_table(test_result_t *results, int count) {
    printf("\n========================================================\n");
    printf("          Page Replacement Algorithm Comparison\n");
    printf("========================================================\n");
    printf("Algorithm    | Frames | Faults | Hits | Fault Rate\n");
    printf("-------------+--------+--------+------+------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-12s | %6d | %6d | %4d | %9.2f%%\n",
               results[i].algorithm_name,
               results[i].frame_count,
               results[i].page_fault_count,
               results[i].page_hit_count,
               results[i].page_fault_rate);
    }
    
    printf("========================================================\n\n");
}

// 运行单个算法测试
test_result_t run_algorithm_test(algorithm_type_t algo_type, int *sequence, 
                                  int length, int frame_count) {
    replacement_manager_t rm;
    init_replacement_manager(&rm, frame_count);
    
    test_result_t result;
    result.frame_count = frame_count;
    result.sequence_length = length;
    
    switch (algo_type) {
        case ALGORITHM_FIFO:
            result.algorithm_name = "FIFO";
            fifo_algorithm(&rm, sequence, length);
            break;
        case ALGORITHM_LRU:
            result.algorithm_name = "LRU";
            lru_algorithm(&rm, sequence, length);
            break;
        case ALGORITHM_OPTIMAL:
            result.algorithm_name = "OPTIMAL";
            optimal_algorithm(&rm, sequence, length);
            break;
    }
    
    result.page_fault_count = rm.page_fault_count;
    result.page_hit_count = rm.page_hit_count;
    result.page_fault_rate = get_page_fault_rate(&rm);
    
    return result;
}

// 运行对比测试
void run_comparison_test(int *sequence, int length, int *frame_counts, 
                         int frame_count_size) {
    printf("\n========================================================\n");
    printf("  Page Replacement Algorithm Comparison Test\n");
    printf("========================================================\n");
    
    printf("\nTest Sequence: [");
    for (int i = 0; i < length; i++) {
        printf("%d", sequence[i]);
        if (i < length - 1) printf(", ");
    }
    printf("]\n");
    printf("Sequence Length: %d\n\n", length);
    
    // 对每种帧数进行测试
    for (int i = 0; i < frame_count_size; i++) {
        int frames = frame_counts[i];
        
        printf("========== Testing with %d frames ==========\n\n", frames);
        
        test_result_t results[3];
        results[0] = run_algorithm_test(ALGORITHM_FIFO, sequence, length, frames);
        results[1] = run_algorithm_test(ALGORITHM_LRU, sequence, length, frames);
        results[2] = run_algorithm_test(ALGORITHM_OPTIMAL, sequence, length, frames);
        
        print_comparison_table(results, 3);
        
        // 分析结果
        printf("Analysis for %d frames:\n", frames);
        printf("  - Best Algorithm: %s (Fault Rate: %.2f%%)\n",
               results[2].algorithm_name, results[2].page_fault_rate);
        
        if (results[1].page_fault_count < results[0].page_fault_count) {
            printf("  - LRU performs better than FIFO\n");
            printf("    (%.2f%% fewer page faults)\n",
                   (double)(results[0].page_fault_count - results[1].page_fault_count) /
                   results[0].page_fault_count * 100);
        } else if (results[1].page_fault_count > results[0].page_fault_count) {
            printf("  - FIFO performs better than LRU in this case\n");
        } else {
            printf("  - LRU and FIFO have equal performance\n");
        }
        
        printf("  - OPTIMAL is %.2f%% better than LRU\n",
               (double)(results[1].page_fault_count - results[2].page_fault_count) /
               results[1].page_fault_count * 100);
        
        printf("\n");
    }
}

