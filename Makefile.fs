# 简易文件系统实验 Makefile

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
TARGET = test_fs
OBJS = simple_fs.o test_fs.o

# 默认目标
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS)
	@echo "正在链接 $(TARGET)..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "编译完成！可执行文件：$(TARGET)"

# 编译 simple_fs.c
simple_fs.o: simple_fs.c simple_fs.h
	@echo "正在编译 simple_fs.c..."
	$(CC) $(CFLAGS) -c simple_fs.c

# 编译 test_fs.c
test_fs.o: test_fs.c simple_fs.h
	@echo "正在编译 test_fs.c..."
	$(CC) $(CFLAGS) -c test_fs.c

# 运行测试
run: $(TARGET)
	@echo "=========================================="
	@echo "运行测试程序..."
	@echo "=========================================="
	./$(TARGET)

# 清理编译产物
clean:
	@echo "清理编译产物..."
	rm -f $(OBJS) $(TARGET)
	@echo "清理完成！"

# 使用 Valgrind 检查内存泄漏（Linux only）
valgrind: $(TARGET)
	@echo "运行内存检查..."
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# 帮助信息
help:
	@echo "可用的 Make 目标："
	@echo "  make          - 编译程序"
	@echo "  make run      - 编译并运行测试"
	@echo "  make clean    - 清理编译产物"
	@echo "  make valgrind - 使用 Valgrind 检查内存（Linux）"
	@echo "  make help     - 显示此帮助信息"

.PHONY: all run clean valgrind help

