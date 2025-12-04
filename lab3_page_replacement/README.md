# 实验三：页面置换算法比较

## 实验概述

本实验实现并比较三种经典的页面置换算法：
- **FIFO（先进先出）**：最简单的置换算法
- **LRU（最近最少使用）**：基于局部性原理的实用算法
- **OPTIMAL（最优置换）**：理论最优算法，作为性能基准

## 文件结构

```
lab3_page_replacement/
├── page_replacement.h      # 头文件
├── page_replacement.c      # 核心实现
├── test.c                 # 测试程序
├── Makefile               # 编译配置
└── README.md              # 本文件
```

## 算法特性分析

### 1. FIFO（先进先出）算法

**基本思想**：选择最早进入内存的页面进行置换

**实现方式**：
- 维护页面的加载时间戳（load_time）
- 选择load_time最小的页面置换

**优点**：
- ✅ 实现简单
- ✅ 开销小
- ✅ 公平性好

**缺点**：
- ❌ 性能一般
- ❌ 存在Belady异常（增加帧数可能增加缺页）
- ❌ 可能置换常用页面

**时间复杂度**：O(n)，n为帧数

### 2. LRU（最近最少使用）算法

**基本思想**：选择最久未使用的页面进行置换

**实现方式**：
- 维护页面的最后访问时间戳（last_access_time）
- 每次访问时更新时间戳
- 选择last_access_time最小的页面置换

**优点**：
- ✅ 基于局部性原理
- ✅ 性能较好
- ✅ 不存在Belady异常

**缺点**：
- ❌ 实现相对复杂
- ❌ 每次访问需更新时间戳
- ❌ 硬件实现开销大

**时间复杂度**：O(n)

### 3. OPTIMAL（最优置换）算法

**基本思想**：选择未来最长时间不使用的页面进行置换

**实现方式**：
- 需要预知未来的访问序列
- 对每个页面，查找其下一次使用的位置
- 选择下一次使用位置最远（或不再使用）的页面

**优点**：
- ✅ 理论最优，缺页率最低
- ✅ 作为性能比较基准

**缺点**：
- ❌ 需要预知未来访问序列
- ❌ 实际系统无法实现
- ❌ 仅用于理论分析

**时间复杂度**：O(n × m)，n为帧数，m为剩余序列长度

## 编译和运行

### 编译
```bash
cd lab3_page_replacement
make
```

### 运行测试
```bash
make run
# 或者
./test_replacement
```

### 清理
```bash
make clean
```

## 测试说明

### 标准测试序列

根据实验要求，使用以下测试配置：

**测试序列**：`{1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5}`

**帧数配置**：3帧、4帧、5帧

### 测试内容

#### 1. 主要对比测试
- 在3、4、5帧条件下分别测试三种算法
- 记录缺页次数、命中次数、缺页率
- 生成性能对比表格

#### 2. 详细步骤分析
- 展示每一步的访问过程
- 显示帧的实时状态
- 标注命中/缺页情况

#### 3. Belady异常测试
- 验证FIFO算法的Belady异常
- 展示增加帧数反而增加缺页的情况

#### 4. 局部性原理测试
- 使用具有良好局部性的访问序列
- 验证LRU在局部性好的情况下表现优秀

#### 5. 随机访问测试
- 使用随机访问序列
- 分析算法在随机模式下的表现

## 输出信息

### 对比表格示例
```
========================================================
          Page Replacement Algorithm Comparison
========================================================
Algorithm    | Frames | Faults | Hits | Fault Rate
-------------+--------+--------+------+------------
FIFO         |      3 |      9 |    3 |     75.00%
LRU          |      3 |      7 |    5 |     58.33%
OPTIMAL      |      3 |      6 |    6 |     50.00%
========================================================
```

### 详细步骤示例
```
Step | Page | Status | Frame Contents
-----+------+--------+------------------
   1 |    1 | FAULT  | 1 - - 
   2 |    2 | FAULT  | 1 2 - 
   3 |    3 | FAULT  | 1 2 3 
   4 |    4 | FAULT  | 4 2 3 
   5 |    1 | FAULT  | 4 1 3 
   6 |    2 | FAULT  | 4 1 2 
   7 |    5 | FAULT  | 5 1 2 
   8 |    1 |  HIT   | 5 1 2 
   9 |    2 |  HIT   | 5 1 2 
  10 |    3 | FAULT  | 5 3 2 
  11 |    4 | FAULT  | 5 3 4 
  12 |    5 |  HIT   | 5 3 4 
```

### 统计信息
```
========== Statistics ==========
Total Accesses: 12
Page Faults: 9
Page Hits: 3
Page Fault Rate: 75.00%
Hit Rate: 25.00%
================================
```

## 性能分析

### 预期结果（标准测试序列，3帧）

| 算法 | 缺页次数 | 缺页率 | 特点 |
|------|---------|--------|------|
| FIFO | 9 | 75.00% | 性能最差 |
| LRU | 7-8 | 58-67% | 性能中等 |
| OPTIMAL | 6-7 | 50-58% | 性能最优 |

### 关键发现

1. **OPTIMAL总是最优**：
   - 作为理论上限，缺页率最低
   - 其他算法无法超越

2. **LRU通常优于FIFO**：
   - 基于局部性原理
   - 在大多数实际场景中表现更好

3. **帧数增加，缺页率下降**：
   - 一般规律：更多帧 → 更少缺页
   - 例外：FIFO的Belady异常

4. **算法选择取决于访问模式**：
   - 良好局部性：LRU表现优异
   - 随机访问：FIFO和LRU差异不大
   - 顺序访问：FIFO可能足够

## 算法实现要点

### FIFO实现
```c
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
```

### LRU实现
```c
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
```

### OPTIMAL实现
```c
int optimal_select_victim(replacement_manager_t *rm, int *sequence, 
                          int current_pos, int length) {
    int victim = 0;
    int farthest = -1;
    
    for (int i = 0; i < rm->frame_count; i++) {
        int page = rm->frames[i].page_num;
        int next_use = INT_MAX;
        
        // 查找下一次使用位置
        for (int j = current_pos + 1; j < length; j++) {
            if (sequence[j] == page) {
                next_use = j;
                break;
            }
        }
        
        // 选择最晚使用的页面
        if (next_use > farthest) {
            farthest = next_use;
            victim = i;
        }
    }
    
    return victim;
}
```

## Belady异常详解

**定义**：增加物理帧数反而导致缺页次数增加的现象

**示例**：
```
序列：1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5

3帧时：9次缺页
4帧时：10次缺页（更多！）
```

**原因**：
- FIFO只考虑进入顺序，不考虑使用频率
- 增加帧数改变了页面进入顺序
- 可能导致常用页面被过早置换

**结论**：
- 不是所有算法都有Belady异常
- LRU和OPTIMAL不存在此问题
- 说明FIFO的局限性

## 扩展实验

### 1. 实现其他算法
- **Clock算法**：改进的FIFO
- **Second Chance算法**：给页面第二次机会
- **LFU算法**：最不经常使用

### 2. 性能优化
- 使用栈或队列优化LRU
- 实现硬件辅助的LRU近似算法

### 3. 实际场景模拟
- 数据库访问模式
- 编译器访问模式
- 视频播放访问模式

## 学习目标

通过本实验，您将：
1. ✅ 理解三种经典页面置换算法的原理
2. ✅ 掌握算法的实现方法
3. ✅ 学会性能分析和比较
4. ✅ 理解Belady异常现象
5. ✅ 认识局部性原理的重要性

## 常见问题

### Q1：为什么OPTIMAL不能在实际系统中使用？
A：因为它需要预知未来的访问序列，这在实际系统中是不可能的。

### Q2：LRU为什么比FIFO好？
A：LRU基于局部性原理，最近使用的页面很可能再次被使用，因此保留最近使用的页面是合理的。

### Q3：如何选择合适的页面置换算法？
A：考虑：
- 实现复杂度
- 性能要求
- 访问模式特点
- 硬件支持

一般情况下，LRU近似算法（如Clock）是很好的折中选择。

---

**实验完成标准**：
- ✅ 三种算法正确实现
- ✅ 测试用例全部通过
- ✅ 性能对比清晰
- ✅ Belady异常得到验证
- ✅ 统计数据准确

祝实验顺利！

