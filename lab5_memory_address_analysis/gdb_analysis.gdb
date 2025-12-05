# GDB 分析脚本
# 用于在 GDB 中分析程序内存布局

# 设置反汇编风格为 Intel 格式
set disassembly-flavor intel

echo \n========================================\n
echo   GDB Memory Analysis Session\n
echo ========================================\n

# 在 main 函数设置断点
break main

echo Starting program...\n
run

echo \n========================================\n
echo 1. Function Addresses (Code Section)\n
echo ========================================\n

print &main
print &add
print &increment_point

echo \n========================================\n
echo 2. Global Variables (Data Section)\n
echo ========================================\n

print &global_var
print &static_global_var

echo \n========================================\n
echo 3. Local Variables in main (Stack)\n
echo ========================================\n

# 继续执行到设置局部变量后
break func_demo.c:48
continue

print &stack_var
print &sum
print &heap_point

echo \n========================================\n
echo 4. Analyzing add() Function Call\n
echo ========================================\n

# 在 add 函数设置断点
break add
continue

# 显示 add 函数的反汇编
disassemble add

# 显示栈帧信息
info frame
info locals
info args

# 显示参数地址
print &a
print &b
print &result

echo \n========================================\n
echo 5. Analyzing increment_point() Function Call\n
echo ========================================\n

# 在 increment_point 函数设置断点
break increment_point
continue

# 显示 increment_point 函数的反汇编
disassemble increment_point

# 显示栈帧信息
info frame
info locals
info args

# 显示参数（指针）和指向的内容
print p
print *p
print &p

echo \n========================================\n
echo 6. Heap Memory Analysis\n
echo ========================================\n

# 继续执行到 malloc 后
break func_demo.c:70
continue

print heap_point
print *heap_point
print &heap_point

echo \n========================================\n
echo 7. Stack Frame Comparison\n
echo ========================================\n

# 显示所有栈帧
info stack

# 切换到 main 的栈帧
frame 0
info frame
info locals

# 切换到 add 的栈帧（如果还在）
frame 1
info frame
info locals

echo \n========================================\n
echo Analysis Complete!\n
echo ========================================\n

# 退出
quit

