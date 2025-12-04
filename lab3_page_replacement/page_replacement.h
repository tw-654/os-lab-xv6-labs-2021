#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include <stdbool.h>

#define MAX_FRAMES 10       // 最大物理帧数
#define MAX_SEQUENCE 100    // 最大访问序列长度

// 页面置换算法类型
typedef enum {
    ALGORITHM_FIFO,
    ALGORITHM_LRU,
    ALGORITHM_OPTIMAL
} algorithm_type_t;

// 物理帧结构
typedef struct {
    int page_num;           // 存储的页面号（-1表示空闲）
    int load_time;          // 加载时间（用于FIFO）
    int last_access_time;   // 最后访问时间（用于LRU）
} frame_t;

// 页面置换管理器
typedef struct {
    frame_t frames[MAX_FRAMES];     // 物理帧数组
    int frame_count;                // 物理帧数量
    int current_time;               // 当前时间戳
    int page_fault_count;           // 缺页次数
    int page_hit_count;             // 命中次数
    int total_access_count;         // 总访问次数
} replacement_manager_t;

// 测试结果结构
typedef struct {
    const char *algorithm_name;     // 算法名称
    int frame_count;                // 帧数
    int sequence_length;            // 序列长度
    int page_fault_count;           // 缺页次数
    int page_hit_count;             // 命中次数
    double page_fault_rate;         // 缺页率
} test_result_t;

// 初始化和销毁
void init_replacement_manager(replacement_manager_t *rm, int frame_count);
void reset_replacement_manager(replacement_manager_t *rm);

// 三种页面置换算法
int fifo_algorithm(replacement_manager_t *rm, int *sequence, int length);
int lru_algorithm(replacement_manager_t *rm, int *sequence, int length);
int optimal_algorithm(replacement_manager_t *rm, int *sequence, int length);

// 辅助函数
int find_page_in_frames(replacement_manager_t *rm, int page_num);
int find_free_frame(replacement_manager_t *rm);
int fifo_select_victim(replacement_manager_t *rm);
int lru_select_victim(replacement_manager_t *rm);
int optimal_select_victim(replacement_manager_t *rm, int *sequence, int current_pos, int length);
void load_page(replacement_manager_t *rm, int frame_index, int page_num);
void access_page(replacement_manager_t *rm, int frame_index);

// 统计和显示
void print_frame_status(replacement_manager_t *rm);
void print_statistics(replacement_manager_t *rm);
double get_page_fault_rate(replacement_manager_t *rm);
void print_test_result(test_result_t *result);
void print_comparison_table(test_result_t *results, int count);

// 性能测试
test_result_t run_algorithm_test(algorithm_type_t algo_type, int *sequence, 
                                  int length, int frame_count);
void run_comparison_test(int *sequence, int length, int *frame_counts, 
                         int frame_count_size);

#endif // PAGE_REPLACEMENT_H

