#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 增强测试：展示算法差异
void test_algorithm_differences() {
    printf("\n========== Advanced Test: Algorithm Differences ==========\n");
    printf("This test demonstrates how the three algorithms behave differently\n");
    printf("when multiple free partitions exist.\n\n");
    
    memory_manager_t mm_ff, mm_bf, mm_wf;
    
    // 初始化三个内存管理器
    init_memory_manager(&mm_ff, 256);
    init_memory_manager(&mm_bf, 256);
    init_memory_manager(&mm_wf, 256);
    
    // 创建初始状态：多个空闲分区
    // 先分配一些内存，然后释放中间的部分，形成多个分区
    printf("--- Step 1: Create multiple free partitions ---\n");
    
    // First Fit
    int addr1_ff = first_fit_allocate(&mm_ff, 50);
    int addr2_ff = first_fit_allocate(&mm_ff, 30);
    int addr3_ff = first_fit_allocate(&mm_ff, 40);
    int addr4_ff = first_fit_allocate(&mm_ff, 60);
    release_memory(&mm_ff, addr2_ff, 30);  // 释放中间的分区
    
    // Best Fit
    int addr1_bf = best_fit_allocate(&mm_bf, 50);
    int addr2_bf = best_fit_allocate(&mm_bf, 30);
    int addr3_bf = best_fit_allocate(&mm_bf, 40);
    int addr4_bf = best_fit_allocate(&mm_bf, 60);
    release_memory(&mm_bf, addr2_bf, 30);
    
    // Worst Fit
    int addr1_wf = worst_fit_allocate(&mm_wf, 50);
    int addr2_wf = worst_fit_allocate(&mm_wf, 30);
    int addr3_wf = worst_fit_allocate(&mm_wf, 40);
    int addr4_wf = worst_fit_allocate(&mm_wf, 60);
    release_memory(&mm_wf, addr2_wf, 30);
    
    printf("Initial state created. Now we have multiple free partitions.\n\n");
    
    // 现在尝试分配25 KB，观察三种算法的选择
    printf("--- Step 2: Allocate 25 KB (can fit in multiple partitions) ---\n");
    
    int addr_ff = first_fit_allocate(&mm_ff, 25);
    int addr_bf = best_fit_allocate(&mm_bf, 25);
    int addr_wf = worst_fit_allocate(&mm_wf, 25);
    
    printf("\nFirst Fit allocated at: %d\n", addr_ff);
    printf("Best Fit allocated at: %d\n", addr_bf);
    printf("Worst Fit allocated at: %d\n", addr_wf);
    
    printf("\n--- First Fit Memory Status ---\n");
    print_memory_status(&mm_ff);
    
    printf("\n--- Best Fit Memory Status ---\n");
    print_memory_status(&mm_bf);
    
    printf("\n--- Worst Fit Memory Status ---\n");
    print_memory_status(&mm_wf);
    
    // 清理
    destroy_memory_manager(&mm_ff);
    destroy_memory_manager(&mm_bf);
    destroy_memory_manager(&mm_wf);
}

// 测试内存不足场景
void test_insufficient_memory() {
    printf("\n========== Test: Insufficient Memory Scenario ==========\n");
    
    memory_manager_t mm;
    init_memory_manager(&mm, 100);  // 只有100 KB
    
    printf("Total memory: 100 KB\n");
    printf("Allocation sequence: 50, 30, 40, 20 KB (total: 140 KB)\n\n");
    
    int addrs[4];
    int sizes[4] = {50, 30, 40, 20};
    
    for (int i = 0; i < 4; i++) {
        int addr = first_fit_allocate(&mm, sizes[i]);
        if (addr != -1) {
            addrs[i] = addr;
            printf("Allocate %d KB: SUCCESS at address %d\n", sizes[i], addr);
        } else {
            addrs[i] = -1;
            printf("Allocate %d KB: FAILED (insufficient memory)\n", sizes[i]);
        }
        print_memory_status(&mm);
    }
    
    destroy_memory_manager(&mm);
}

// 测试非顺序释放
void test_non_sequential_release() {
    printf("\n========== Test: Non-Sequential Release ==========\n");
    
    memory_manager_t mm;
    init_memory_manager(&mm, 256);
    
    printf("Allocation sequence: 50, 30, 20, 40, 60, 10, 25 KB\n");
    printf("Release sequence: 20, 40, 50, 30, 10, 60, 25 KB (non-sequential)\n\n");
    
    // 分配
    int addrs[7];
    int sizes[7] = {50, 30, 20, 40, 60, 10, 25};
    for (int i = 0; i < 7; i++) {
        addrs[i] = first_fit_allocate(&mm, sizes[i]);
        printf("Allocate %d KB: address %d\n", sizes[i], addrs[i]);
    }
    
    printf("\n--- Release Phase (Non-Sequential) ---\n");
    
    // 非顺序释放：释放索引2, 3, 0, 1, 5, 4, 6
    int release_order[7] = {2, 3, 0, 1, 5, 4, 6};
    for (int i = 0; i < 7; i++) {
        int idx = release_order[i];
        printf("\nReleasing block %d: [%d, %d) size=%d KB\n", 
               idx, addrs[idx], addrs[idx] + sizes[idx], sizes[idx]);
        release_memory(&mm, addrs[idx], sizes[idx]);
        print_memory_status(&mm);
    }
    
    destroy_memory_manager(&mm);
}

int main() {
    printf("==========================================\n");
    printf("  Advanced Memory Management Tests\n");
    printf("==========================================\n");
    
    // 测试1：算法差异
    test_algorithm_differences();
    
    // 测试2：内存不足
    test_insufficient_memory();
    
    // 测试3：非顺序释放
    test_non_sequential_release();
    
    printf("\nAll advanced tests completed!\n");
    return 0;
}

