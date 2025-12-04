#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 测试序列
#define TEST_SIZE 7
int test_sequence[TEST_SIZE] = {50, 30, 20, 40, 60, 10, 25}; // KB

// 已分配内存块结构（用于测试）
typedef struct {
    int addr;
    int size;
} allocated_info_t;

// 测试函数
void test_algorithm(const char *algorithm_name, 
                    int (*allocate_func)(memory_manager_t *, int),
                    memory_manager_t *mm) {
    printf("\n========== Testing %s Algorithm ==========\n", algorithm_name);
    
    // 重新初始化内存管理器
    destroy_memory_manager(mm);
    init_memory_manager(mm, 256); // 假设总内存为256KB
    
    allocated_info_t allocated[TEST_SIZE];
    int allocated_count = 0;
    int success_count = 0;
    int fail_count = 0;
    
    // 执行分配操作
    printf("\n--- Allocation Phase ---\n");
    for (int i = 0; i < TEST_SIZE; i++) {
        int addr = allocate_func(mm, test_sequence[i]);
        if (addr != -1) {
            allocated[allocated_count].addr = addr;
            allocated[allocated_count].size = test_sequence[i];
            allocated_count++;
            success_count++;
            printf("Allocate %d KB: SUCCESS at address %d\n", test_sequence[i], addr);
        } else {
            fail_count++;
            printf("Allocate %d KB: FAILED (no suitable partition)\n", test_sequence[i]);
        }
        print_memory_status(mm);
    }
    
    // 执行释放操作（释放所有已分配的内存）
    printf("\n--- Release Phase ---\n");
    for (int i = 0; i < allocated_count; i++) {
        if (release_memory(mm, allocated[i].addr, allocated[i].size)) {
            printf("Release [%d, %d) size=%d KB: SUCCESS\n", 
                   allocated[i].addr, allocated[i].addr + allocated[i].size, allocated[i].size);
        } else {
            printf("Release [%d, %d) size=%d KB: FAILED\n", 
                   allocated[i].addr, allocated[i].addr + allocated[i].size, allocated[i].size);
        }
        print_memory_status(mm);
    }
    
    // 统计结果
    printf("\n--- Statistics ---\n");
    printf("Total Allocation Requests: %d\n", TEST_SIZE);
    printf("Successful Allocations: %d\n", success_count);
    printf("Failed Allocations: %d\n", fail_count);
    printf("Success Rate: %.2f%%\n", (float)success_count / TEST_SIZE * 100);
    printf("Final Fragmentation: %d KB\n", calculate_fragmentation(mm));
    printf("Final Free Memory: %d KB\n", get_free_memory(mm));
    printf("==========================================\n\n");
}

int main() {
    printf("==========================================\n");
    printf("  Dynamic Partition Allocation Algorithm\n");
    printf("         Experiment 1 Test Suite\n");
    printf("==========================================\n");
    
    memory_manager_t mm;
    
    // 测试首次适应算法
    test_algorithm("First Fit", first_fit_allocate, &mm);
    
    // 测试最佳适应算法
    test_algorithm("Best Fit", best_fit_allocate, &mm);
    
    // 测试最坏适应算法
    test_algorithm("Worst Fit", worst_fit_allocate, &mm);
    
    // 清理
    destroy_memory_manager(&mm);
    
    printf("\nAll tests completed!\n");
    return 0;
}

