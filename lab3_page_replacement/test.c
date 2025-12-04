#include "page_replacement.h"
#include <stdio.h>
#include <stdlib.h>

// 详细测试单个算法
void test_algorithm_detailed(const char *algo_name, algorithm_type_t algo_type,
                              int *sequence, int length, int frame_count) {
    printf("\n========================================\n");
    printf("Testing %s Algorithm (Detailed)\n", algo_name);
    printf("========================================\n");
    printf("Frames: %d, Sequence Length: %d\n\n", frame_count, length);
    
    replacement_manager_t rm;
    init_replacement_manager(&rm, frame_count);
    
    printf("Access Sequence:\n");
    printf("Step | Page | Status | Frame Contents\n");
    printf("-----+------+--------+------------------\n");
    
    for (int i = 0; i < length; i++) {
        int page = sequence[i];
        rm.current_time++;
        rm.total_access_count++;
        
        int frame_index = find_page_in_frames(&rm, page);
        
        if (frame_index != -1) {
            // 页面命中
            rm.page_hit_count++;
            if (algo_type == ALGORITHM_LRU) {
                access_page(&rm, frame_index);
            }
            printf("%4d | %4d |  HIT   | ", i + 1, page);
        } else {
            // 缺页
            rm.page_fault_count++;
            
            frame_index = find_free_frame(&rm);
            
            if (frame_index == -1) {
                // 需要置换
                switch (algo_type) {
                    case ALGORITHM_FIFO:
                        frame_index = fifo_select_victim(&rm);
                        break;
                    case ALGORITHM_LRU:
                        frame_index = lru_select_victim(&rm);
                        break;
                    case ALGORITHM_OPTIMAL:
                        frame_index = optimal_select_victim(&rm, sequence, i, length);
                        break;
                }
            }
            
            load_page(&rm, frame_index, page);
            printf("%4d | %4d | FAULT  | ", i + 1, page);
        }
        
        // 打印帧内容
        for (int j = 0; j < frame_count; j++) {
            if (rm.frames[j].page_num == -1) {
                printf("- ");
            } else {
                printf("%d ", rm.frames[j].page_num);
            }
        }
        printf("\n");
    }
    
    print_statistics(&rm);
}

// 测试Belady异常
void test_belady_anomaly() {
    printf("\n========================================\n");
    printf("Testing Belady's Anomaly\n");
    printf("========================================\n");
    printf("Demonstrating that FIFO can have more page faults\n");
    printf("with more frames (Belady's Anomaly)\n\n");
    
    // 会产生Belady异常的访问序列
    int sequence[] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};
    int length = sizeof(sequence) / sizeof(sequence[0]);
    
    printf("Test Sequence: [");
    for (int i = 0; i < length; i++) {
        printf("%d", sequence[i]);
        if (i < length - 1) printf(", ");
    }
    printf("]\n\n");
    
    // 测试3帧和4帧的情况
    int frame_counts[] = {3, 4};
    
    for (int i = 0; i < 2; i++) {
        test_result_t result = run_algorithm_test(ALGORITHM_FIFO, sequence, 
                                                   length, frame_counts[i]);
        printf("FIFO with %d frames: %d page faults (%.2f%%)\n",
               frame_counts[i], result.page_fault_count, result.page_fault_rate);
    }
    
    printf("\n");
}

// 测试局部性原理
void test_locality() {
    printf("\n========================================\n");
    printf("Testing Locality Principle\n");
    printf("========================================\n");
    printf("Testing with sequence that has good locality\n\n");
    
    // 具有良好局部性的访问序列
    int sequence[] = {1, 2, 1, 2, 1, 2, 3, 4, 3, 4, 3, 4};
    int length = sizeof(sequence) / sizeof(sequence[0]);
    int frame_count = 3;
    
    printf("Test Sequence: [");
    for (int i = 0; i < length; i++) {
        printf("%d", sequence[i]);
        if (i < length - 1) printf(", ");
    }
    printf("]\n\n");
    
    test_result_t results[3];
    results[0] = run_algorithm_test(ALGORITHM_FIFO, sequence, length, frame_count);
    results[1] = run_algorithm_test(ALGORITHM_LRU, sequence, length, frame_count);
    results[2] = run_algorithm_test(ALGORITHM_OPTIMAL, sequence, length, frame_count);
    
    print_comparison_table(results, 3);
    
    printf("Analysis: With good locality, LRU should perform well\n");
    printf("because recently used pages are likely to be used again.\n\n");
}

// 测试随机访问模式
void test_random_pattern() {
    printf("\n========================================\n");
    printf("Testing Random Access Pattern\n");
    printf("========================================\n");
    
    // 随机访问模式
    int sequence[] = {1, 5, 2, 7, 3, 1, 4, 6, 2, 8, 5, 3};
    int length = sizeof(sequence) / sizeof(sequence[0]);
    int frame_count = 4;
    
    printf("Test Sequence (Random): [");
    for (int i = 0; i < length; i++) {
        printf("%d", sequence[i]);
        if (i < length - 1) printf(", ");
    }
    printf("]\n\n");
    
    test_result_t results[3];
    results[0] = run_algorithm_test(ALGORITHM_FIFO, sequence, length, frame_count);
    results[1] = run_algorithm_test(ALGORITHM_LRU, sequence, length, frame_count);
    results[2] = run_algorithm_test(ALGORITHM_OPTIMAL, sequence, length, frame_count);
    
    print_comparison_table(results, 3);
    
    printf("Analysis: With random access, FIFO and LRU may have\n");
    printf("similar performance. OPTIMAL still performs best.\n\n");
}

int main() {
    printf("==========================================\n");
    printf("  Page Replacement Algorithm Comparison\n");
    printf("       Experiment 3 Test Suite\n");
    printf("==========================================\n");
    
    // 实验要求的测试序列
    int standard_sequence[] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};
    int standard_length = sizeof(standard_sequence) / sizeof(standard_sequence[0]);
    
    // 实验要求的帧数
    int frame_counts[] = {3, 4, 5};
    int frame_count_size = sizeof(frame_counts) / sizeof(frame_counts[0]);
    
    // 主要对比测试
    run_comparison_test(standard_sequence, standard_length, 
                        frame_counts, frame_count_size);
    
    // 详细测试：展示每一步的过程（使用3帧）
    printf("\n========== Detailed Step-by-Step Analysis ==========\n");
    test_algorithm_detailed("FIFO", ALGORITHM_FIFO, 
                           standard_sequence, standard_length, 3);
    test_algorithm_detailed("LRU", ALGORITHM_LRU, 
                           standard_sequence, standard_length, 3);
    test_algorithm_detailed("OPTIMAL", ALGORITHM_OPTIMAL, 
                           standard_sequence, standard_length, 3);
    
    // 额外测试
    test_belady_anomaly();
    test_locality();
    test_random_pattern();
    
    printf("\n==========================================\n");
    printf("All tests completed!\n");
    printf("==========================================\n");
    
    return 0;
}

