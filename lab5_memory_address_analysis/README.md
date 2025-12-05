# 实验五：程序内存地址分配分析

## 实验目的

分析程序在内存中的布局，理解代码区、数据区、堆栈区和堆的使用情况，通过反汇编观察函数如何处理局部变量、参数和返回值。

## 实验内容

### 6.1 实验原理

程序被调度到内存执行后，会分配具体的内存空间，包含：

1. **代码区 (Text/Code Segment)**
   - 存储可执行指令
   - 只读
   - 通常位于低地址区域

2. **数据区 (Data Segment)**
   - 存储初始化的全局变量和静态变量
   - 可读写

3. **BSS 区 (Block Started by Symbol)**
   - 存储未初始化的全局变量和静态变量
   - 程序启动时自动初始化为0

4. **堆区 (Heap)**
   - 动态分配的内存（malloc/free）
   - 向上增长

5. **栈区 (Stack)**
   - 局部变量、函数参数、返回地址
   - 向下增长（从高地址向低地址）

### 6.2 示例程序

`func_demo.c` 演示了：

- **函数 1：`add(int a, int b)`**
  - 简单参数传递（值传递）
  - 简单返回值
  - 局部变量的使用

- **函数 2：`increment_point(struct Point *p)`**
  - 指针参数传递（引用传递）
  - 修改堆内存中的数据
  - 无返回值

- **主函数：`main()`**
  - 堆内存分配（malloc）
  - 堆内存释放（free）
  - 各种内存区域的变量

## 编译和运行

### 编译

```bash
make
```

这将编译两个程序：
- `func_demo`：演示程序，显示内存地址
- `memory_analysis`：内存分析程序

### 运行演示程序

```bash
make demo
# 或
./func_demo
```

### 运行内存分析

```bash
make analysis
# 或
./memory_analysis
```

### 综合分析

运行自动分析脚本（需要 objdump/readelf）：

```bash
make analyze
# 或
chmod +x analyze.sh
./analyze.sh
```

### 反汇编

生成反汇编代码：

```bash
make disasm
# 生成的汇编代码保存在 func_demo.asm
```

### 使用 GDB 分析

```bash
make gdb
# 或手动运行
gdb -batch -x gdb_analysis.gdb func_demo
```

## 学生任务

### 任务 6.3.1：内存分配分析

分析程序在 Linux 系统中的内存使用情况：

1. **代码区分析**
   - 观察函数的地址范围
   - 分析代码区的特点（只读、共享等）

2. **数据区分析**
   - 全局变量的地址
   - 静态变量的地址
   - 数据区的布局

3. **堆栈区分析**
   - 局部变量的地址
   - 函数参数的地址
   - 栈的增长方向

4. **堆区分析**
   - 动态分配内存的地址
   - 堆的增长方向
   - malloc/free 的影响

5. **反汇编分析**
   - 观察函数如何传递参数
   - 观察局部变量如何分配
   - 观察返回值如何传递

## 分析方法

### 方法 1：程序运行时分析

运行 `func_demo` 程序，观察输出的内存地址：

```bash
./func_demo
```

分析要点：
- 代码区（函数地址）通常是最小的地址
- 数据区（全局变量）地址较小
- 栈区（局部变量）地址较大（接近高地址）
- 堆区（malloc 分配）地址在栈和数据区之间

### 方法 2：使用 objdump 反汇编

```bash
# 查看节信息
objdump -h func_demo

# 反汇编特定函数
objdump -d func_demo | grep -A 20 "<add>:"

# 查看符号表
objdump -t func_demo
```

### 方法 3：使用 readelf 查看 ELF 结构

```bash
# 查看节头表
readelf -S func_demo

# 查看程序头表（内存段）
readelf -l func_demo

# 查看符号表
readelf -s func_demo
```

### 方法 4：使用 GDB 调试分析

```bash
gdb func_demo
(gdb) break main
(gdb) run
(gdb) print &global_var
(gdb) print &local_var
(gdb) disassemble add
(gdb) info frame
(gdb) info locals
(gdb) info args
```

### 方法 5：使用 nm 查看符号

```bash
nm func_demo
```

符号类型：
- `T` / `t`：代码段中的符号（函数）
- `D` / `d`：数据段中的符号（已初始化全局变量）
- `B` / `b`：BSS 段中的符号（未初始化全局变量）

## 内存布局示意图

```
高地址 (0x7fffffffffff)
    ↓
  [栈 (Stack)]
    - 局部变量
    - 函数参数
    - 返回地址
    - 向下增长
    ↓
  [内存映射段]
    - 共享库
    - mmap 映射
    ↓
  [堆 (Heap)]
    - malloc/free 分配
    - 向上增长
    ↓
  [BSS 段]
    - 未初始化全局/静态变量
    ↓
  [数据段 (Data)]
    - 已初始化全局/静态变量
    ↓
  [代码段 (Text)]
    - 可执行代码
    - 只读
    ↓
低地址 (0x400000)
```

## 函数调用分析

### 值传递（Pass by Value）

```c
int add(int a, int b) {
    int result;
    result = a + b;
    return result;
}
```

- 参数 `a` 和 `b` 的值被复制到栈上
- 函数内部修改不影响原始变量
- 返回值通过寄存器（如 EAX/RAX）传递

### 指针传递（Pass by Reference）

```c
void increment_point(struct Point *p) {
    p->x += 1;
    p->y += 1;
}
```

- 参数 `p` 是指针，存储的是地址
- 通过指针可以修改指向的内存
- 函数内部修改会影响原始数据

### 栈帧结构

```
高地址
    ↓
  [前一个栈帧的帧指针]
  [返回地址]
  [参数（从右到左）]
  [局部变量]
    ↓
低地址（栈顶）
```

## 实验报告要求

1. **内存布局分析**
   - 列出各内存区域的地址范围
   - 分析地址的分布规律

2. **函数调用分析**
   - 分析 `add()` 函数的参数传递方式
   - 分析返回值如何传递
   - 分析栈帧的结构

3. **堆内存分析**
   - 分析 `malloc()` 分配的地址特点
   - 分析堆内存的使用情况

4. **反汇编分析**
   - 提供关键函数的反汇编代码
   - 解释关键指令的作用
   - 分析参数和局部变量的存储位置

5. **总结和思考**
   - 总结不同内存区域的特点
   - 分析函数调用对内存的影响
   - 思考内存管理的重要性

## 文件结构

```
lab5_memory_address_analysis/
├── func_demo.c          # 示例程序
├── memory_analysis.c    # 内存分析工具
├── analyze.sh          # 自动分析脚本
├── gdb_analysis.gdb    # GDB 分析脚本
├── Makefile            # 编译脚本
└── README.md           # 本文件
```

## 注意事项

1. 内存地址会因系统和编译选项而有所不同
2. 使用 `-g` 编译选项以便调试
3. 使用 `-fno-omit-frame-pointer` 以便栈帧分析
4. 不同的编译器可能产生不同的汇编代码
5. 地址空间布局随机化（ASLR）可能影响地址

## 扩展实验

1. 分析递归函数对栈的影响
2. 分析大结构体的传递方式
3. 分析不同优化级别对代码的影响（-O0, -O1, -O2, -O3）
4. 比较 32 位和 64 位程序的内存布局差异

## 参考资料

- `man objdump`
- `man readelf`
- `man nm`
- `man gdb`
- ELF 文件格式规范
- x86-64 ABI 规范

