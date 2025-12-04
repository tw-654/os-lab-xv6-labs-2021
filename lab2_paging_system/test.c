#include "paging_system.h"
#include <stdio.h>
#include <stdlib.h>

// 测试基本地址转换
void test_basic_translation() {
    printf("\n========================================\n");
    printf("Test 1: Basic Address Translation\n");
    printf("========================================\n");
    
    paging_system_t ps;
    init_paging_system(&ps);
    
    // 测试序列：访问不同页面
    int test_addresses[] = {
        0,      // Page 0, Offset 0
        4096,   // Page 1, Offset 0
        8192,   // Page 2, Offset 0
        100,    // Page 0, Offset 100
        4196    // Page 1, Offset 100
    };
    
    char operations[] = {'R', 'R', 'W', 'R', 'W'};
    
    for (int i = 0; i < 5; i++) {
        translate_address(&ps, test_addresses[i], operations[i]);
    }
    
    print_page_table(&ps);
    print_frame_info(&ps);
    print_statistics(&ps);
    
    destroy_paging_system(&ps);
}

// 测试FIFO页面置换
void test_fifo_replacement() {
    printf("\n========================================\n");
    printf("Test 2: FIFO Page Replacement\n");
    printf("========================================\n");
    printf("Physical frames: %d, Testing with 5 pages\n", MAX_FRAMES);
    printf("Expected: Page replacement will occur\n\n");
    
    paging_system_t ps;
    init_paging_system(&ps);
    
    // 访问5个不同的页面（超过物理帧数4）
    // 页面访问序列：0, 1, 2, 3, 4
    int page_sequence[] = {0, 1, 2, 3, 4};
    
    for (int i = 0; i < 5; i++) {
        int logical_addr = page_sequence[i] * PAGE_SIZE;
        translate_address(&ps, logical_addr, 'R');
    }
    
    printf("\n--- After initial access sequence ---\n");
    print_page_table(&ps);
    print_frame_info(&ps);
    
    // 再次访问页面0（应该导致缺页，因为页面0已被置换）
    printf("\n--- Accessing page 0 again (should cause page fault) ---\n");
    translate_address(&ps, 0, 'R');
    
    print_page_table(&ps);
    print_frame_info(&ps);
    print_statistics(&ps);
    
    destroy_paging_system(&ps);
}

// 测试修改位和写回
void test_modified_bit() {
    printf("\n========================================\n");
    printf("Test 3: Modified Bit and Write-back\n");
    printf("========================================\n");
    
    paging_system_t ps;
    init_paging_system(&ps);
    
    // 访问页面0-3（填满物理内存）
    for (int i = 0; i < MAX_FRAMES; i++) {
        translate_address(&ps, i * PAGE_SIZE, 'R');
    }
    
    // 写入页面0和2
    printf("\n--- Writing to pages 0 and 2 ---\n");
    translate_address(&ps, 0, 'W');
    translate_address(&ps, 2 * PAGE_SIZE, 'W');
    
    print_page_table(&ps);
    
    // 访问新页面4（将触发FIFO置换，页面0将被置换并写回）
    printf("\n--- Accessing new page 4 (will replace page 0) ---\n");
    translate_address(&ps, 4 * PAGE_SIZE, 'R');
    
    print_page_table(&ps);
    print_frame_info(&ps);
    print_statistics(&ps);
    
    destroy_paging_system(&ps);
}

// 测试复杂访问序列
void test_complex_sequence() {
    printf("\n========================================\n");
    printf("Test 4: Complex Access Sequence\n");
    printf("========================================\n");
    printf("Testing reference string: 0, 1, 2, 3, 0, 1, 4, 0, 1, 2, 3, 4\n\n");
    
    paging_system_t ps;
    init_paging_system(&ps);
    
    // 经典的页面访问序列
    int reference_string[] = {0, 1, 2, 3, 0, 1, 4, 0, 1, 2, 3, 4};
    int length = sizeof(reference_string) / sizeof(reference_string[0]);
    
    for (int i = 0; i < length; i++) {
        int logical_addr = reference_string[i] * PAGE_SIZE;
        translate_address(&ps, logical_addr, (i % 3 == 0) ? 'W' : 'R');
    }
    
    print_page_table(&ps);
    print_frame_info(&ps);
    print_statistics(&ps);
    
    destroy_paging_system(&ps);
}

// 测试页内偏移
void test_page_offset() {
    printf("\n========================================\n");
    printf("Test 5: Page Offset Translation\n");
    printf("========================================\n");
    
    paging_system_t ps;
    init_paging_system(&ps);
    
    // 测试不同的页内偏移
    int test_cases[] = {
        0,          // Page 0, Offset 0
        100,        // Page 0, Offset 100
        4095,       // Page 0, Offset 4095
        4096,       // Page 1, Offset 0
        8192 + 500  // Page 2, Offset 500
    };
    
    for (int i = 0; i < 5; i++) {
        translate_address(&ps, test_cases[i], 'R');
    }
    
    print_page_table(&ps);
    print_statistics(&ps);
    
    destroy_paging_system(&ps);
}

int main() {
    printf("==========================================\n");
    printf("  Paging System Address Translation\n");
    printf("       Experiment 2 Test Suite\n");
    printf("==========================================\n");
    
    // 运行所有测试
    test_basic_translation();
    test_fifo_replacement();
    test_modified_bit();
    test_complex_sequence();
    test_page_offset();
    
    printf("\n==========================================\n");
    printf("All tests completed!\n");
    printf("==========================================\n");
    
    return 0;
}

