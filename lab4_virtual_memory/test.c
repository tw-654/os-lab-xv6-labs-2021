#include "virtual_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_ACCESSES 1000  // 1000次内存访问

// 测试结果结构
typedef struct {
    const char *pattern_name;
    int total_accesses;
    int page_faults;
    int disk_io_count;
    double page_fault_rate;
    double avg_disk_io_per_fault;
} test_result_t;

// 生成具有局部性的访问序列
void generate_locality_sequence(int *sequence, int length) {
    // 模拟程序访问模式：大部分时间访问少量页面（局部性原理）
    srand((unsigned int)time(NULL));
    
    // 定义几个热点页面
    int hot_pages[] = {10, 11, 12, 20, 21, 30, 31, 32};
    int num_hot = sizeof(hot_pages) / sizeof(hot_pages[0]);
    
    // 70% 访问热点页面，30% 随机访问
    for (int i = 0; i < length; i++) {
        if (rand() % 100 < 70) {
            // 访问热点页面
            sequence[i] = hot_pages[rand() % num_hot];
        } else {
            // 随机访问其他页面
            sequence[i] = rand() % 100;  // 0-99页
        }
    }
}

// 生成随机访问序列
void generate_random_sequence(int *sequence, int length) {
    srand((unsigned int)time(NULL) + 1);
    
    for (int i = 0; i < length; i++) {
        sequence[i] = rand() % 200;  // 随机访问0-199页
    }
}

// 生成顺序访问序列
void generate_sequential_sequence(int *sequence, int length) {
    for (int i = 0; i < length; i++) {
        sequence[i] = (i / 10) % 100;  // 每10次访问换一个页面
    }
}

// 生成工作集访问序列（模拟程序的工作集变化）
void generate_working_set_sequence(int *sequence, int length) {
    srand((unsigned int)time(NULL) + 2);
    
    int current_working_set = 0;
    int working_set_size = 8;
    
    for (int i = 0; i < length; i++) {
        // 每200次访问切换工作集
        if (i % 200 == 0) {
            current_working_set = (current_working_set + 1) % 4;
        }
        
        // 在工作集内随机访问
        int base_page = current_working_set * 25;
        sequence[i] = base_page + (rand() % working_set_size);
    }
}

// 运行测试模式
test_result_t run_test_pattern(virtual_memory_system_t *vms, 
                               const char *pattern_name,
                               void (*generate_fn)(int *, int),
                               int *sequence) {
    // 重置虚拟内存系统
    init_virtual_memory_system(vms);
    
    // 生成访问序列
    generate_fn(sequence, NUM_ACCESSES);
    
    // 随机生成操作类型（70%读，30%写）
    srand((unsigned int)time(NULL) + 3);
    
    // 执行内存访问
    int page_faults_before = vms->page_faults;
    int disk_io_before = vms->disk_io_count;
    
    for (int i = 0; i < NUM_ACCESSES; i++) {
        int logical_addr = sequence[i] * PAGE_SIZE + (rand() % PAGE_SIZE);
        char operation = (rand() % 100 < 70) ? 'R' : 'W';
        
        int physical_addr = vm_translate(vms, logical_addr, operation);
        
        if (physical_addr < 0) {
            printf("  [ERROR] Translation failed at access %d\n", i);
        }
    }
    
    test_result_t result;
    result.pattern_name = pattern_name;
    result.total_accesses = NUM_ACCESSES;
    result.page_faults = vms->page_faults - page_faults_before;
    result.disk_io_count = vms->disk_io_count - disk_io_before;
    
    // 计算缺页率
    if (NUM_ACCESSES > 0) {
        result.page_fault_rate = (double)result.page_faults / NUM_ACCESSES * 100.0;
    } else {
        result.page_fault_rate = 0.0;
    }
    
    if (result.page_faults > 0) {
        result.avg_disk_io_per_fault = (double)result.disk_io_count / result.page_faults;
    } else {
        result.avg_disk_io_per_fault = 0.0;
    }
    
    return result;
}

// 打印测试结果
void print_test_result(const test_result_t *result) {
    printf("  Pattern:            %s\n", result->pattern_name);
    printf("  Total Accesses:     %d\n", result->total_accesses);
    printf("  Page Faults:        %d\n", result->page_faults);
    printf("  Page Fault Rate:    %.2f%%\n", result->page_fault_rate);
    printf("  Disk I/O Count:     %d\n", result->disk_io_count);
    printf("  Avg I/O per Fault:  %.2f\n", result->avg_disk_io_per_fault);
    printf("\n");
}

// 打印对比表
void print_comparison_table(test_result_t *results, int count) {
    printf("\n========================================\n");
    printf("Performance Comparison Table\n");
    printf("========================================\n");
    printf("%-20s | %8s | %10s | %12s | %15s\n", 
           "Pattern", "Faults", "Fault Rate", "Disk I/O", "Avg I/O/Fault");
    printf("---------------------+----------+------------+--------------+-----------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-20s | %8d | %9.2f%% | %12d | %15.2f\n",
               results[i].pattern_name,
               results[i].page_faults,
               results[i].page_fault_rate,
               results[i].disk_io_count,
               results[i].avg_disk_io_per_fault);
    }
    printf("\n");
}

// 详细跟踪前100次访问
void detailed_trace_first_accesses(virtual_memory_system_t *vms, 
                                   int *sequence, int num_trace) {
    init_virtual_memory_system(vms);
    
    printf("\n========================================\n");
    printf("Detailed Trace of First %d Accesses\n", num_trace);
    printf("========================================\n");
    printf("Access | Logical Addr | Page | Operation | Physical Addr | Status | Faults\n");
    printf("-------+--------------+------+-----------+---------------+--------+--------\n");
    
    srand((unsigned int)time(NULL) + 4);
    
    for (int i = 0; i < num_trace && i < NUM_ACCESSES; i++) {
        int page_num = sequence[i];
        int offset = rand() % PAGE_SIZE;
        int logical_addr = page_num * PAGE_SIZE + offset;
        char operation = (rand() % 100 < 70) ? 'R' : 'W';
        
        int faults_before = vms->page_faults;
        int physical_addr = vm_translate(vms, logical_addr, operation);
        int faults_after = vms->page_faults;
        
        const char *status = (faults_after > faults_before) ? "FAULT" : "HIT";
        
        printf("%6d | 0x%010X | %4d | %9c | 0x%011X | %6s | %6d\n",
               i + 1,
               logical_addr,
               page_num,
               operation,
               physical_addr,
               status,
               vms->page_faults);
    }
    
    printf("\n");
    print_physical_memory_status(vms);
}

// 性能分析
void performance_analysis(test_result_t *results, int count) {
    printf("\n========================================\n");
    printf("Performance Analysis\n");
    printf("========================================\n");
    
    // 找到最优和最差表现
    int best_idx = 0, worst_idx = 0;
    double best_rate = results[0].page_fault_rate;
    double worst_rate = results[0].page_fault_rate;
    
    for (int i = 1; i < count; i++) {
        if (results[i].page_fault_rate < best_rate) {
            best_rate = results[i].page_fault_rate;
            best_idx = i;
        }
        if (results[i].page_fault_rate > worst_rate) {
            worst_rate = results[i].page_fault_rate;
            worst_idx = i;
        }
    }
    
    printf("Best Performance:   %s (%.2f%% fault rate)\n", 
           results[best_idx].pattern_name, best_rate);
    printf("Worst Performance:  %s (%.2f%% fault rate)\n", 
           results[worst_idx].pattern_name, worst_rate);
    
    printf("\nKey Observations:\n");
    printf("1. Locality Pattern: Shows the benefit of temporal and spatial locality.\n");
    printf("   Pages that are accessed recently are likely to be accessed again.\n");
    printf("2. Random Pattern: Higher page fault rate due to lack of locality.\n");
    printf("3. Sequential Pattern: Moderate performance, depends on access stride.\n");
    printf("4. Working Set Pattern: Shows how working set changes affect performance.\n");
    printf("\n");
}

int main() {
    printf("==========================================\n");
    printf("  Virtual Memory System Comprehensive Test\n");
    printf("       Experiment 4 Test Suite\n");
    printf("==========================================\n");
    printf("\n");
    printf("System Configuration:\n");
    printf("  - Physical Memory: %d frames x %d KB = %d KB\n",
           MAX_PHYSICAL_FRAMES, PAGE_SIZE / 1024,
           MAX_PHYSICAL_FRAMES * PAGE_SIZE / 1024);
    printf("  - Virtual Address Space: %d pages x %d KB = %d KB\n",
           MAX_PAGES, PAGE_SIZE / 1024,
           MAX_PAGES * PAGE_SIZE / 1024);
    printf("  - Test Accesses: %d\n", NUM_ACCESSES);
    printf("\n");
    
    virtual_memory_system_t vms;
    int sequence[NUM_ACCESSES];
    test_result_t results[4];
    
    // 测试1：局部性访问模式
    printf("========================================\n");
    printf("Test 1: Locality Access Pattern\n");
    printf("========================================\n");
    printf("Simulating program with good temporal and spatial locality.\n");
    printf("Most accesses (70%%) are to hot pages.\n\n");
    results[0] = run_test_pattern(&vms, "Locality", 
                                  generate_locality_sequence, sequence);
    print_test_result(&results[0]);
    
    // 测试2：随机访问模式
    printf("========================================\n");
    printf("Test 2: Random Access Pattern\n");
    printf("========================================\n");
    printf("Simulating random memory access pattern.\n");
    printf("No locality expected.\n\n");
    results[1] = run_test_pattern(&vms, "Random", 
                                  generate_random_sequence, sequence);
    print_test_result(&results[1]);
    
    // 测试3：顺序访问模式
    printf("========================================\n");
    printf("Test 3: Sequential Access Pattern\n");
    printf("========================================\n");
    printf("Simulating sequential memory access pattern.\n\n");
    results[2] = run_test_pattern(&vms, "Sequential", 
                                  generate_sequential_sequence, sequence);
    print_test_result(&results[2]);
    
    // 测试4：工作集变化模式
    printf("========================================\n");
    printf("Test 4: Working Set Variation Pattern\n");
    printf("========================================\n");
    printf("Simulating program with changing working sets.\n");
    printf("Working set changes every 200 accesses.\n\n");
    results[3] = run_test_pattern(&vms, "Working Set", 
                                  generate_working_set_sequence, sequence);
    print_test_result(&results[3]);
    
    // 打印对比表
    print_comparison_table(results, 4);
    
    // 详细跟踪（局部性模式的前100次访问）
    generate_locality_sequence(sequence, NUM_ACCESSES);
    detailed_trace_first_accesses(&vms, sequence, 100);
    
    // 性能分析
    performance_analysis(results, 4);
    
    // 最终统计
    printf("========================================\n");
    printf("Final Statistics Summary\n");
    printf("========================================\n");
    print_statistics(&vms);
    
    printf("==========================================\n");
    printf("All tests completed!\n");
    printf("==========================================\n");
    
    return 0;
}

