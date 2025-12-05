#include <stdio.h>
#include <stdlib.h>  // 引入 malloc 和 free

// 全局变量（数据区）
int global_var = 100;
static int static_global_var = 200;

// 函数 1：简单参数，简单返回值
int add(int a, int b) {
    int result;  // 局部变量（栈区）
    result = a + b;
    return result;
}

// 结构体定义
struct Point {
    int x;
    int y;
};

// 函数 2：结构体指针参数（指向堆内存），无返回值
// 功能：将一个点的 x 和 y 坐标都增加 1
void increment_point(struct Point *p) {
    if (p == NULL) {
        printf("Error: Null pointer received!\n");
        return;
    }
    p->x += 1;
    p->y += 1;
}

// 辅助函数：打印内存地址信息
void print_memory_info(const char *name, void *addr) {
    printf("  %-20s: %p\n", name, addr);
}

int main() {
    // 局部变量（栈区）
    int sum;
    struct Point *heap_point;
    int stack_var = 42;
    
    printf("========================================\n");
    printf("Memory Address Analysis\n");
    printf("========================================\n\n");
    
    // 打印代码区地址（函数地址）
    printf("Code Section (Function Addresses):\n");
    print_memory_info("main()", (void *)main);
    print_memory_info("add()", (void *)add);
    print_memory_info("increment_point()", (void *)increment_point);
    print_memory_info("print_memory_info()", (void *)print_memory_info);
    printf("\n");
    
    // 打印数据区地址（全局变量）
    printf("Data Section (Global Variables):\n");
    print_memory_info("global_var", (void *)&global_var);
    print_memory_info("static_global_var", (void *)&static_global_var);
    printf("\n");
    
    // 打印栈区地址（局部变量）
    printf("Stack Section (Local Variables in main):\n");
    print_memory_info("stack_var", (void *)&stack_var);
    print_memory_info("sum", (void *)&sum);
    print_memory_info("heap_point", (void *)&heap_point);
    printf("\n");
    
    // 函数调用：简单参数和返回值
    printf("Function Call: add(3, 5)\n");
    int a = 3, b = 5;
    printf("  Parameter addresses:\n");
    print_memory_info("  &a (in main)", (void *)&a);
    print_memory_info("  &b (in main)", (void *)&b);
    
    sum = add(3, 5);
    printf("  Sum: %d\n", sum);
    print_memory_info("  &sum (in main)", (void *)&sum);
    printf("\n");
    
    // 堆内存分配
    printf("Heap Memory Allocation:\n");
    heap_point = (struct Point *)malloc(sizeof(struct Point));
    if (heap_point == NULL) {
        perror("malloc failed");
        return 1;
    }
    
    heap_point->x = 100;
    heap_point->y = 200;
    
    printf("  Before increment (Heap): x = %d, y = %d\n", 
           heap_point->x, heap_point->y);
    print_memory_info("  heap_point (pointer)", (void *)&heap_point);
    print_memory_info("  *heap_point (heap)", (void *)heap_point);
    printf("\n");
    
    // 函数调用：结构体指针参数
    printf("Function Call: increment_point(heap_point)\n");
    printf("  Parameter address:\n");
    print_memory_info("  heap_point (in main)", (void *)&heap_point);
    printf("  Pointed to address:\n");
    print_memory_info("  *heap_point", (void *)heap_point);
    
    increment_point(heap_point);
    
    printf("  After increment (Heap): x = %d, y = %d\n", 
           heap_point->x, heap_point->y);
    printf("\n");
    
    // 释放堆内存
    printf("Freeing Heap Memory:\n");
    free(heap_point);
    heap_point = NULL;
    print_memory_info("  heap_point (after free)", (void *)&heap_point);
    printf("\n");
    
    printf("========================================\n");
    printf("Memory Layout Summary\n");
    printf("========================================\n");
    printf("Code Section:    Lowest addresses (read-only)\n");
    printf("Data Section:    Global/static variables\n");
    printf("Heap Section:    Dynamically allocated memory (grows upward)\n");
    printf("Stack Section:   Local variables, parameters (grows downward)\n");
    printf("========================================\n");
    
    return 0;
}

