#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// 内存区域分析结构
typedef struct {
    const char *name;
    void *start_addr;
    void *end_addr;
    size_t size;
    const char *description;
} memory_region_t;

// 打印内存区域信息
void print_memory_region(const memory_region_t *region) {
    if (region->start_addr && region->end_addr) {
        printf("%-20s: %p - %p (%zu bytes) - %s\n",
               region->name,
               region->start_addr,
               region->end_addr,
               region->size,
               region->description);
    } else {
        printf("%-20s: %s\n", region->name, region->description);
    }
}

// 前向声明
void analyze_function_call();
int main();

// 分析程序的内存布局
void analyze_memory_layout() {
    printf("========================================\n");
    printf("Program Memory Layout Analysis\n");
    printf("========================================\n\n");
    
    // 代码区（函数地址示例）
    printf("Memory Regions:\n");
    printf("------------------------------------------------------------\n");
    printf("%-20s: %p (example: analyze_memory_layout)\n", "Code (Text)", (void *)analyze_memory_layout);
    printf("%-20s: %p (example: main)\n", "", (void *)main);
    printf("%-20s: %p (example: analyze_function_call)\n", "", (void *)analyze_function_call);
    printf("\n");
    
    // 堆栈信息（运行时地址）
    int stack_var = 0;
    static int static_var = 0;
    void *heap_var = malloc(1024);
    
    printf("Runtime Memory Addresses:\n");
    printf("------------------------------------------------------------\n");
    printf("%-20s: %p (local variable - grows downward)\n", "Stack", (void *)&stack_var);
    printf("%-20s: %p (static variable)\n", "Static/Data", (void *)&static_var);
    printf("%-20s: %p (malloc allocated - grows upward)\n", "Heap", heap_var);
    printf("\n");
    
    free(heap_var);
}

// 分析函数调用的内存使用
void analyze_function_call() {
    printf("========================================\n");
    printf("Function Call Memory Analysis\n");
    printf("========================================\n\n");
    
    printf("Stack Frame Structure:\n");
    printf("------------------------------------------------------------\n");
    printf("High Address (Stack Bottom)\n");
    printf("  ↓\n");
    printf("  [Previous Frame Pointer]\n");
    printf("  [Return Address]\n");
    printf("  [Function Parameters (right to left)]\n");
    printf("  [Local Variables]\n");
    printf("  ↓\n");
    printf("Low Address (Stack Top)\n");
    printf("\n");
    
    printf("Example: int add(int a, int b)\n");
    printf("------------------------------------------------------------\n");
    printf("Stack Layout:\n");
    printf("  [Local variable: result]  <- Lower address\n");
    printf("  [Parameter: b]\n");
    printf("  [Parameter: a]\n");
    printf("  [Return Address]\n");
    printf("  [Previous Frame Pointer] <- Higher address\n");
    printf("\n");
}

// 分析参数传递方式
void analyze_parameter_passing() {
    printf("========================================\n");
    printf("Parameter Passing Analysis\n");
    printf("========================================\n\n");
    
    printf("1. Value Passing (Pass by Value):\n");
    printf("   - Parameters are copied to stack\n");
    printf("   - Changes inside function don't affect original\n");
    printf("   - Example: int add(int a, int b)\n");
    printf("\n");
    
    printf("2. Pointer Passing (Pass by Reference):\n");
    printf("   - Address of variable is passed\n");
    printf("   - Changes inside function affect original\n");
    printf("   - Example: void increment_point(struct Point *p)\n");
    printf("\n");
    
    printf("3. Return Value:\n");
    printf("   - Usually returned in register (e.g., EAX/RAX on x86/x86_64)\n");
    printf("   - For large structures, may be returned via pointer\n");
    printf("\n");
}

// 打印内存地址范围分析
void print_memory_address_ranges() {
    printf("========================================\n");
    printf("Typical Memory Layout (Linux x86_64)\n");
    printf("========================================\n\n");
    
    printf("High Address (0x7fffffffffff)\n");
    printf("  ↓\n");
    printf("  [Kernel Space]             0xffff800000000000 - 0xffffffffffffffff\n");
    printf("  (Reserved for kernel)\n");
    printf("  ↓\n");
    printf("  [Stack]                    0x7fff00000000 - (grows downward)\n");
    printf("  (Local variables, parameters)\n");
    printf("  ↓\n");
    printf("  [Memory Mapping Segment]   (mmap, shared libraries)\n");
    printf("  ↓\n");
    printf("  [Heap]                     (grows upward) - 0x600000\n");
    printf("  (Dynamic allocation)\n");
    printf("  ↓\n");
    printf("  [BSS]                      Uninitialized global/static\n");
    printf("  ↓\n");
    printf("  [Data]                     Initialized global/static\n");
    printf("  ↓\n");
    printf("  [Text/Code]                0x400000 - (read-only)\n");
    printf("  (Program code)\n");
    printf("  ↓\n");
    printf("Low Address (0x400000)\n");
    printf("\n");
}

int main() {
    printf("\n");
    analyze_memory_layout();
    printf("\n");
    analyze_function_call();
    printf("\n");
    analyze_parameter_passing();
    printf("\n");
    print_memory_address_ranges();
    
    return 0;
}

