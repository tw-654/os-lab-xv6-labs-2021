@echo off
REM Windows 编译脚本

echo ========================================
echo 简易文件系统实验 - Windows 编译脚本
echo ========================================
echo.

REM 检查是否安装了 GCC
where gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误：未找到 GCC 编译器！
    echo 请安装 MinGW 或 TDM-GCC
    pause
    exit /b 1
)

echo [1/3] 编译 simple_fs.c...
gcc -Wall -Wextra -g -std=c99 -c simple_fs.c
if %errorlevel% neq 0 (
    echo 编译失败！
    pause
    exit /b 1
)

echo [2/3] 编译 test_fs.c...
gcc -Wall -Wextra -g -std=c99 -c test_fs.c
if %errorlevel% neq 0 (
    echo 编译失败！
    pause
    exit /b 1
)

echo [3/3] 链接生成可执行文件...
gcc -Wall -Wextra -g -std=c99 -o test_fs.exe simple_fs.o test_fs.o
if %errorlevel% neq 0 (
    echo 链接失败！
    pause
    exit /b 1
)

echo.
echo ========================================
echo 编译成功！生成文件：test_fs.exe
echo ========================================
echo.

REM 询问是否运行
set /p run="是否立即运行测试程序？(Y/N): "
if /i "%run%"=="Y" (
    echo.
    echo 运行测试程序...
    echo ========================================
    test_fs.exe
    echo ========================================
)

pause

