# PowerShell 编译脚本
# 使用方法：在 PowerShell 中运行：.\build.ps1

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  简易文件系统实验 - 编译脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 检查 GCC 是否安装
$gccPath = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gccPath) {
    Write-Host "错误：未找到 GCC 编译器！" -ForegroundColor Red
    Write-Host "请安装 MinGW 或 TDM-GCC" -ForegroundColor Yellow
    Write-Host "下载地址：https://sourceforge.net/projects/mingw-w64/" -ForegroundColor Yellow
    exit 1
}

Write-Host "[1/3] 编译 simple_fs.c..." -ForegroundColor Green
gcc -Wall -Wextra -g -std=c99 -c simple_fs.c
if ($LASTEXITCODE -ne 0) {
    Write-Host "编译失败！" -ForegroundColor Red
    exit 1
}

Write-Host "[2/3] 编译 test_fs.c..." -ForegroundColor Green
gcc -Wall -Wextra -g -std=c99 -c test_fs.c
if ($LASTEXITCODE -ne 0) {
    Write-Host "编译失败！" -ForegroundColor Red
    exit 1
}

Write-Host "[3/3] 链接生成可执行文件..." -ForegroundColor Green
gcc -Wall -Wextra -g -std=c99 -o test_fs.exe simple_fs.o test_fs.o
if ($LASTEXITCODE -ne 0) {
    Write-Host "链接失败！" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "编译成功！生成文件：test_fs.exe" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""

# 询问是否运行
$run = Read-Host "是否立即运行测试程序？(Y/N)"
if ($run -eq "Y" -or $run -eq "y") {
    Write-Host ""
    Write-Host "运行测试程序..." -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    .\test_fs.exe
    Write-Host "========================================" -ForegroundColor Cyan
}

Write-Host ""
Write-Host "按任意键退出..." -ForegroundColor Gray
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

