#include "simple_fs.h"

// ========== 初始化函数（已提供，无需修改） ==========

/**
 * 初始化文件系统
 * 功能：将所有FCB标记为无效，清空磁盘内容，初始化位示图为全0（空闲）
 */
void init_fs(simple_fs_t* fs) {
    // 初始化FCB表
    for (int i = 0; i < MAX_FILES; i++) {
        fs->fcb_table[i].is_valid = 0;
        memset(fs->fcb_table[i].name, 0, MAX_FILENAME_LEN);
        fs->fcb_table[i].size = 0;
        fs->fcb_table[i].start_block = -1;
    }
    
    // 初始化磁盘内容为0
    memset(fs->disk, 0, DISK_SIZE);
    
    // 初始化位示图（所有块都空闲）
    for (int i = 0; i < BLOCK_NUM; i++) {
        fs->bit_map[i] = 0;
    }
    
    printf("文件系统初始化成功！\n");
    printf("磁盘大小：%d KB (%d 字节)\n", DISK_SIZE / 1024, DISK_SIZE);
    printf("块大小：%d 字节\n", BLOCK_SIZE);
    printf("总块数：%d\n", BLOCK_NUM);
    printf("最大文件数：%d\n\n", MAX_FILES);
}

// ========== 任务1：实现空闲块查找函数 ==========

/**
 * 查找连续的空闲块
 * @param fs 文件系统指针
 * @param block_count 需要的连续块数量
 * @return 成功返回起始块号，失败返回-1
 */
int find_free_blocks(simple_fs_t* fs, int block_count) {
    // TODO: 学生需要完成以下步骤:
    // 1. 遍历所有可能的起始块位置（从0到BLOCK_NUM - block_count）
    // 2. 对每个起始位置，检查其后的block_count个块是否都空闲（bit_map值为0）
    // 3. 如果找到满足条件的连续空闲块，返回起始块号
    // 4. 如果遍历完仍未找到，返回-1表示失败
    
    for (int start = 0; start <= BLOCK_NUM - block_count; start++) {
        int found = 1;
        for (int i = 0; i < block_count; i++) {
            int current_block = start + i;
            // 学生需要在此处添加检查逻辑
            // 检查当前块是否空闲（bit_map值为0表示空闲）
            if (fs->bit_map[current_block] != 0) {
                found = 0;  // 发现占用的块，不符合要求
                break;      // 跳出内层循环，检查下一个起始位置
            }
        }
        if (found) {
            printf("找到空闲块：起始块=%d, 块数=%d\n", start, block_count);
            return start;
        }
    }
    printf("错误：找不到连续的%d个空闲块\n", block_count);
    return -1;
}

// ========== 任务2：实现文件创建函数 ==========

/**
 * 创建文件
 * @param fs 文件系统指针
 * @param filename 文件名
 * @param size 文件大小（字节）
 * @return 成功返回文件描述符(fd)，失败返回-1
 */
int my_create(simple_fs_t* fs, const char* filename, int size) {
    // 参数检查
    if (filename == NULL || strlen(filename) == 0) {
        printf("错误：文件名为空！\n");
        return -1;
    }
    if (size <= 0) {
        printf("错误：文件大小必须大于0！\n");
        return -1;
    }
    
    // 检查文件名是否已存在
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->fcb_table[i].is_valid && 
            strcmp(fs->fcb_table[i].name, filename) == 0) {
            printf("错误：文件 %s 已存在！\n", filename);
            return -1;
        }
    }
    
    // 查找空闲的FCB
    int fd = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->fcb_table[i].is_valid == 0) {
            fd = i;
            break;
        }
    }
    if (fd == -1) {
        printf("错误：FCB表已满，无法创建更多文件！\n");
        return -1;
    }
    
    // TODO: 学生需要完成以下步骤:
    // 1. 根据文件大小size，计算需要的磁盘块数量block_count
    // 2. 调用find_free_blocks函数，尝试分配连续的空闲块
    // 3. 如果分配成功，将分配到的块在位示图bit_map中标记为已使用（值设为1）
    // 4. 初始化FCB的各个字段（name, size, start_block, is_valid）
    
    // 计算需要的块数（向上取整）
    int block_count = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    
    // 查找空闲块
    int start_block = find_free_blocks(fs, block_count);
    if (start_block == -1) {
        printf("错误：磁盘空间不足！\n");
        return -1;
    }
    
    // 标记块为已使用
    for (int i = 0; i < block_count; i++) {
        fs->bit_map[start_block + i] = 1;  // 标记为占用
    }
    
    // 初始化FCB
    strncpy(fs->fcb_table[fd].name, filename, MAX_FILENAME_LEN - 1);
    fs->fcb_table[fd].name[MAX_FILENAME_LEN - 1] = '\0';  // 确保字符串结束
    fs->fcb_table[fd].size = size;
    fs->fcb_table[fd].start_block = start_block;
    fs->fcb_table[fd].is_valid = 1;
    
    printf("文件创建成功：%s (FD=%d, 大小=%d字节, 起始块=%d, 占用块数=%d)\n", 
           filename, fd, size, start_block, block_count);
    return fd;
}

// ========== 任务3：实现文件写入函数 ==========

/**
 * 写入文件
 * @param fs 文件系统指针
 * @param fd 文件描述符
 * @param data 要写入的数据
 * @return 成功返回写入字节数，失败返回-1
 */
int my_write(simple_fs_t* fs, int fd, const char* data) {
    // 参数检查
    if (fd < 0 || fd >= MAX_FILES) {
        printf("错误：无效的文件描述符 %d！\n", fd);
        return -1;
    }
    if (fs->fcb_table[fd].is_valid == 0) {
        printf("错误：文件描述符 %d 对应的文件不存在！\n", fd);
        return -1;
    }
    if (data == NULL) {
        printf("错误：写入数据为空！\n");
        return -1;
    }
    
    // TODO: 学生需要完成以下步骤:
    // 1. 通过文件描述符fd找到对应的FCB
    // 2. 计算数据在磁盘中的起始地址（start_block * BLOCK_SIZE）
    // 3. 使用memcpy将数据data拷贝到磁盘内存数组disk的相应位置
    // 4. 注意处理数据长度，不能超过文件大小
    
    fcb_t* fcb = &fs->fcb_table[fd];
    int data_len = strlen(data);
    
    // 限制写入长度不超过文件大小
    if (data_len > fcb->size) {
        data_len = fcb->size;
        printf("警告：数据长度超过文件大小，将截断到 %d 字节\n", data_len);
    }
    
    // 计算起始地址
    int start_address = fcb->start_block * BLOCK_SIZE;
    
    // 写入数据到磁盘
    memcpy(&fs->disk[start_address], data, data_len);
    
    printf("写入成功：向文件 %s 写入 %d 字节\n", fcb->name, data_len);
    return data_len;
}

// ========== 任务4：实现文件读取函数 ==========

/**
 * 读取文件
 * @param fs 文件系统指针
 * @param fd 文件描述符
 * @param buffer 用于存储读取数据的缓冲区
 * @param buffer_size 缓冲区大小
 * @return 成功返回读取字节数，失败返回-1
 */
int my_read(simple_fs_t* fs, int fd, char* buffer, int buffer_size) {
    // 参数检查
    if (fd < 0 || fd >= MAX_FILES) {
        printf("错误：无效的文件描述符 %d！\n", fd);
        return -1;
    }
    if (fs->fcb_table[fd].is_valid == 0) {
        printf("错误：文件描述符 %d 对应的文件不存在！\n", fd);
        return -1;
    }
    if (buffer == NULL || buffer_size <= 0) {
        printf("错误：无效的缓冲区！\n");
        return -1;
    }
    
    // TODO: 学生需要完成以下步骤:
    // 1. 通过文件描述符fd找到对应的FCB
    // 2. 计算数据在磁盘中的起始地址（start_block * BLOCK_SIZE）
    // 3. 使用memcpy从磁盘内存数组disk的相应位置读取数据到buffer
    // 4. 注意buffer大小，防止溢出，确保字符串以'\0'结尾
    
    fcb_t* fcb = &fs->fcb_table[fd];
    
    // 计算实际读取大小（留一个字节给'\0'）
    int read_size = (buffer_size - 1 < fcb->size) ? buffer_size - 1 : fcb->size;
    
    // 计算起始地址
    int start_address = fcb->start_block * BLOCK_SIZE;
    
    // 从磁盘读取数据
    memcpy(buffer, &fs->disk[start_address], read_size);
    
    // 添加字符串结束符
    buffer[read_size] = '\0';
    
    printf("读取成功：从文件 %s 读取 %d 字节\n", fcb->name, read_size);
    return read_size;
}

// ========== 扩展任务1：实现文件删除函数 ==========

/**
 * 删除文件
 * @param fs 文件系统指针
 * @param fd 文件描述符
 * @return 成功返回0，失败返回-1
 */
int my_delete(simple_fs_t* fs, int fd) {
    // 参数检查
    if (fd < 0 || fd >= MAX_FILES) {
        printf("错误：无效的文件描述符 %d！\n", fd);
        return -1;
    }
    if (fs->fcb_table[fd].is_valid == 0) {
        printf("错误：文件描述符 %d 对应的文件不存在！\n", fd);
        return -1;
    }
    
    // TODO: 学生需要完成以下步骤:
    // 1. 通过文件描述符fd找到对应的FCB
    // 2. 计算文件占用的磁盘块数量block_count
    // 3. 遍历这些块，在位示图bit_map中将其标记为空闲（值设为0）
    // 4. 将FCB的is_valid字段标记为0（无效）
    
    fcb_t* fcb = &fs->fcb_table[fd];
    printf("正在删除文件：%s (大小=%d字节, 起始块=%d)\n", 
           fcb->name, fcb->size, fcb->start_block);
    
    // 计算占用的块数
    int block_count = (fcb->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    
    // 释放磁盘块（在位示图中标记为空闲）
    for (int i = 0; i < block_count; i++) {
        fs->bit_map[fcb->start_block + i] = 0;  // 标记为空闲
    }
    
    // 清空磁盘数据（可选，提高安全性）
    int start_address = fcb->start_block * BLOCK_SIZE;
    memset(&fs->disk[start_address], 0, block_count * BLOCK_SIZE);
    
    // 标记FCB无效
    fcb->is_valid = 0;
    
    printf("文件删除成功，释放了 %d 个磁盘块\n", block_count);
    return 0;
}

// ========== 扩展任务2：实现磁盘状态显示函数 ==========

/**
 * 显示磁盘状态
 * @param fs 文件系统指针
 */
void show_disk_status(simple_fs_t* fs) {
    int used_blocks = 0;
    int used_files = 0;
    
    // TODO: 学生需要完成以下步骤:
    // 1. 遍历位示图bit_map，统计已使用的块数(used_blocks)
    // 2. 遍历FCB表fcb_table，统计有效的文件数(used_files)
    // 3. 计算并打印总空间、已用空间、空闲空间、文件数量等信息
    
    // 统计已使用的块数
    for (int i = 0; i < BLOCK_NUM; i++) {
        if (fs->bit_map[i] != 0) {
            used_blocks++;
        }
    }
    
    // 统计有效的文件数
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->fcb_table[i].is_valid) {
            used_files++;
        }
    }
    
    // 打印磁盘状态信息
    printf("\n=== 磁盘状态 ===\n");
    printf("总块数：%d，已使用：%d，空闲：%d\n", 
           BLOCK_NUM, used_blocks, BLOCK_NUM - used_blocks);
    printf("总空间：%d KB，已用：%d KB，空闲：%d KB\n",
           DISK_SIZE / 1024, 
           (used_blocks * BLOCK_SIZE) / 1024,
           ((BLOCK_NUM - used_blocks) * BLOCK_SIZE) / 1024);
    printf("总文件数：%d/%d\n", used_files, MAX_FILES);
    printf("空间使用率：%.1f%%\n", (float)used_blocks / BLOCK_NUM * 100);
    printf("================\n\n");
}

// ========== 辅助函数：列出所有文件 ==========

/**
 * 列出所有文件
 * @param fs 文件系统指针
 */
void list_files(simple_fs_t* fs) {
    printf("\n=== 文件列表 ===\n");
    printf("%-4s %-20s %-10s %-10s %-10s\n", 
           "FD", "文件名", "大小(B)", "起始块", "占用块数");
    printf("-----------------------------------------------------------\n");
    
    int count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->fcb_table[i].is_valid) {
            int block_count = (fs->fcb_table[i].size + BLOCK_SIZE - 1) / BLOCK_SIZE;
            printf("%-4d %-20s %-10d %-10d %-10d\n",
                   i,
                   fs->fcb_table[i].name,
                   fs->fcb_table[i].size,
                   fs->fcb_table[i].start_block,
                   block_count);
            count++;
        }
    }
    
    if (count == 0) {
        printf("(无文件)\n");
    }
    printf("================\n\n");
}

