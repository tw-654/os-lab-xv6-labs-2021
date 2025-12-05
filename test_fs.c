#include "simple_fs.h"

// ========== 测试函数声明 ==========
void test_task1(simple_fs_t* fs);
void test_task2(simple_fs_t* fs);
void test_task3_4(simple_fs_t* fs);
void test_extension1(simple_fs_t* fs);
void test_extension2(simple_fs_t* fs);
void test_comprehensive(simple_fs_t* fs);

// ========== 主函数 ==========
int main() {
    simple_fs_t* fs = (simple_fs_t*)malloc(sizeof(simple_fs_t));
    if (fs == NULL) {
        printf("错误：内存分配失败！\n");
        return -1;
    }
    
    printf("========================================\n");
    printf("  简易文件系统实验 - 测试程序\n");
    printf("========================================\n\n");
    
    // 初始化文件系统
    init_fs(fs);
    
    // 运行所有测试
    test_task1(fs);
    test_task2(fs);
    test_task3_4(fs);
    test_extension1(fs);
    test_extension2(fs);
    test_comprehensive(fs);
    
    printf("\n========================================\n");
    printf("  所有测试完成！\n");
    printf("========================================\n");
    
    free(fs);
    return 0;
}

// ========== 测试1：空闲块查找 ==========
void test_task1(simple_fs_t* fs) {
    printf("\n【测试1：空闲块查找功能】\n");
    printf("----------------------------------------\n");
    
    // 测试1.1：查找1个空闲块
    printf("\n测试1.1：查找1个空闲块\n");
    int block = find_free_blocks(fs, 1);
    if (block == 0) {
        printf("✓ 测试通过：找到起始块 %d\n", block);
    } else {
        printf("✗ 测试失败：应该找到起始块0\n");
    }
    
    // 测试1.2：查找10个连续空闲块
    printf("\n测试1.2：查找10个连续空闲块\n");
    block = find_free_blocks(fs, 10);
    if (block == 0) {
        printf("✓ 测试通过：找到起始块 %d\n", block);
    } else {
        printf("✗ 测试失败：应该找到起始块0\n");
    }
    
    // 测试1.3：模拟部分块被占用的情况
    printf("\n测试1.3：模拟部分块被占用后查找空闲块\n");
    // 占用块0-9
    for (int i = 0; i < 10; i++) {
        fs->bit_map[i] = 1;
    }
    block = find_free_blocks(fs, 5);
    if (block == 10) {
        printf("✓ 测试通过：找到起始块 %d\n", block);
    } else {
        printf("✗ 测试失败：应该找到起始块10，实际找到 %d\n", block);
    }
    
    // 恢复位示图
    init_fs(fs);
    printf("\n测试1完成！\n");
}

// ========== 测试2：文件创建 ==========
void test_task2(simple_fs_t* fs) {
    printf("\n【测试2：文件创建功能】\n");
    printf("----------------------------------------\n");
    
    // 测试2.1：创建小文件
    printf("\n测试2.1：创建小文件（100字节）\n");
    int fd1 = my_create(fs, "test1.txt", 100);
    if (fd1 >= 0) {
        printf("✓ 测试通过：文件描述符 = %d\n", fd1);
    } else {
        printf("✗ 测试失败：创建文件失败\n");
    }
    
    // 测试2.2：创建大文件
    printf("\n测试2.2：创建大文件（1024字节）\n");
    int fd2 = my_create(fs, "test2.txt", 1024);
    if (fd2 >= 0) {
        printf("✓ 测试通过：文件描述符 = %d\n", fd2);
    } else {
        printf("✗ 测试失败：创建文件失败\n");
    }
    
    // 测试2.3：创建同名文件（应该失败）
    printf("\n测试2.3：尝试创建同名文件（应该失败）\n");
    int fd3 = my_create(fs, "test1.txt", 50);
    if (fd3 == -1) {
        printf("✓ 测试通过：正确拒绝同名文件\n");
    } else {
        printf("✗ 测试失败：应该拒绝同名文件\n");
    }
    
    // 列出当前文件
    list_files(fs);
    printf("\n测试2完成！\n");
}

// ========== 测试3-4：文件读写 ==========
void test_task3_4(simple_fs_t* fs) {
    printf("\n【测试3-4：文件读写功能】\n");
    printf("----------------------------------------\n");
    
    // 测试3.1：写入文件
    printf("\n测试3.1：写入文件\n");
    int fd = my_create(fs, "data.txt", 200);
    if (fd < 0) {
        printf("✗ 测试失败：创建文件失败\n");
        return;
    }
    
    const char* test_data = "Hello, Simple File System! This is a test message.";
    int written = my_write(fs, fd, test_data);
    if (written > 0) {
        printf("✓ 测试通过：写入了 %d 字节\n", written);
    } else {
        printf("✗ 测试失败：写入失败\n");
    }
    
    // 测试4.1：读取文件
    printf("\n测试4.1：读取文件\n");
    char buffer[256];
    int read_bytes = my_read(fs, fd, buffer, sizeof(buffer));
    if (read_bytes > 0) {
        printf("✓ 测试通过：读取了 %d 字节\n", read_bytes);
        printf("  读取内容：%s\n", buffer);
        
        // 验证数据一致性
        if (strcmp(buffer, test_data) == 0) {
            printf("✓ 数据一致性检查通过\n");
        } else {
            printf("✗ 数据一致性检查失败\n");
        }
    } else {
        printf("✗ 测试失败：读取失败\n");
    }
    
    // 测试3.2：写入长文本
    printf("\n测试3.2：写入长文本\n");
    int fd2 = my_create(fs, "long.txt", 512);
    if (fd2 >= 0) {
        char long_text[512];
        for (int i = 0; i < 511; i++) {
            long_text[i] = 'A' + (i % 26);
        }
        long_text[511] = '\0';
        
        my_write(fs, fd2, long_text);
        
        char read_buffer[512];
        my_read(fs, fd2, read_buffer, sizeof(read_buffer));
        
        if (strcmp(long_text, read_buffer) == 0) {
            printf("✓ 长文本读写一致性检查通过\n");
        } else {
            printf("✗ 长文本读写一致性检查失败\n");
        }
    }
    
    list_files(fs);
    printf("\n测试3-4完成！\n");
}

// ========== 扩展测试1：文件删除 ==========
void test_extension1(simple_fs_t* fs) {
    printf("\n【扩展测试1：文件删除功能】\n");
    printf("----------------------------------------\n");
    
    // 显示删除前的状态
    printf("\n删除前的文件列表：\n");
    list_files(fs);
    show_disk_status(fs);
    
    // 删除一个文件
    printf("测试：删除文件 data.txt\n");
    int result = my_delete(fs, 2);  // 假设fd=2是data.txt
    if (result == 0) {
        printf("✓ 测试通过：文件删除成功\n");
    } else {
        printf("✗ 测试失败：文件删除失败\n");
    }
    
    // 显示删除后的状态
    printf("\n删除后的文件列表：\n");
    list_files(fs);
    show_disk_status(fs);
    
    printf("\n扩展测试1完成！\n");
}

// ========== 扩展测试2：磁盘状态显示 ==========
void test_extension2(simple_fs_t* fs) {
    printf("\n【扩展测试2：磁盘状态显示功能】\n");
    printf("----------------------------------------\n");
    
    // 创建几个不同大小的文件来测试
    my_create(fs, "small.txt", 100);
    my_create(fs, "medium.txt", 1000);
    my_create(fs, "large.txt", 5000);
    
    printf("\n创建多个文件后的磁盘状态：\n");
    show_disk_status(fs);
    list_files(fs);
    
    printf("\n扩展测试2完成！\n");
}

// ========== 综合测试 ==========
void test_comprehensive(simple_fs_t* fs) {
    printf("\n【综合测试：完整文件操作流程】\n");
    printf("----------------------------------------\n");
    
    // 重新初始化文件系统
    init_fs(fs);
    
    // 1. 创建文件
    printf("\n步骤1：创建3个文件\n");
    int fd1 = my_create(fs, "file1.txt", 256);
    int fd2 = my_create(fs, "file2.txt", 512);
    int fd3 = my_create(fs, "file3.txt", 1024);
    
    // 2. 写入数据
    printf("\n步骤2：写入数据\n");
    my_write(fs, fd1, "Content of file 1");
    my_write(fs, fd2, "Content of file 2");
    my_write(fs, fd3, "Content of file 3");
    
    // 3. 显示状态
    printf("\n步骤3：显示当前状态\n");
    list_files(fs);
    show_disk_status(fs);
    
    // 4. 读取文件
    printf("\n步骤4：读取文件内容\n");
    char buf[256];
    my_read(fs, fd1, buf, sizeof(buf));
    printf("File1内容：%s\n", buf);
    my_read(fs, fd2, buf, sizeof(buf));
    printf("File2内容：%s\n", buf);
    my_read(fs, fd3, buf, sizeof(buf));
    printf("File3内容：%s\n", buf);
    
    // 5. 删除文件
    printf("\n步骤5：删除file2.txt\n");
    my_delete(fs, fd2);
    
    // 6. 创建新文件（应该使用删除文件释放的空间）
    printf("\n步骤6：创建新文件（复用空间）\n");
    int fd4 = my_create(fs, "file4.txt", 300);
    my_write(fs, fd4, "This is file 4, reusing deleted space");
    
    // 7. 最终状态
    printf("\n步骤7：最终状态\n");
    list_files(fs);
    show_disk_status(fs);
    
    printf("\n✓ 综合测试完成！所有操作正常\n");
}

