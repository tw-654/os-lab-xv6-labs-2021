#!/bin/bash

# 内存地址分析脚本
# 用于分析程序的内存布局和反汇编代码

PROGRAM="func_demo"
DEMO_BIN="./func_demo"
ASM_FILE="func_demo.asm"
MAP_FILE="func_demo.map"

echo "=========================================="
echo "  Memory Address Analysis Tool"
echo "  Experiment 5 Analysis Script"
echo "=========================================="
echo ""

# 检查程序是否存在
if [ ! -f "$DEMO_BIN" ]; then
    echo "Error: $DEMO_BIN not found. Please compile first with 'make'"
    exit 1
fi

echo "1. Running Program to Show Memory Addresses..."
echo "------------------------------------------------------------"
$DEMO_BIN
echo ""

echo "2. Extracting Memory Map from Executable..."
echo "------------------------------------------------------------"
if command -v readelf &> /dev/null; then
    echo "Section Headers:"
    readelf -S $DEMO_BIN | head -20
    echo ""
    echo "Program Headers (Memory Segments):"
    readelf -l $DEMO_BIN
    echo ""
else
    echo "readelf not found. Using objdump instead..."
    objdump -h $DEMO_BIN | head -20
    echo ""
fi

echo "3. Disassembling Functions..."
echo "------------------------------------------------------------"
if command -v objdump &> /dev/null; then
    echo "Disassembly of add() function:"
    echo "------------------------------------------------------------"
    objdump -d $DEMO_BIN | grep -A 20 "<add>:"
    echo ""
    
    echo "Disassembly of increment_point() function:"
    echo "------------------------------------------------------------"
    objdump -d $DEMO_BIN | grep -A 25 "<increment_point>:"
    echo ""
    
    echo "Disassembly of main() function:"
    echo "------------------------------------------------------------"
    objdump -d $DEMO_BIN | grep -A 50 "<main>:"
    echo ""
    
    # 保存完整反汇编到文件
    echo "Saving full disassembly to $ASM_FILE..."
    objdump -d $DEMO_BIN > $ASM_FILE
    echo "Full disassembly saved to $ASM_FILE"
    echo ""
else
    echo "objdump not found. Please install binutils."
fi

echo "4. Analyzing Symbol Information..."
echo "------------------------------------------------------------"
if command -v nm &> /dev/null; then
    echo "Symbol Table (functions and variables):"
    nm $DEMO_BIN | grep -E " (T|D|B|t|d|b) " | head -30
    echo ""
    echo "Legend:"
    echo "  T/t = Text (code) section"
    echo "  D/d = Data section"
    echo "  B/b = BSS section"
    echo ""
else
    echo "nm not found. Using objdump --syms instead..."
    objdump --syms $DEMO_BIN | head -30
    echo ""
fi

echo "5. Memory Region Analysis..."
echo "------------------------------------------------------------"
echo "Code Section (.text):"
objdump -h $DEMO_BIN | grep "\.text"
echo ""

echo "Data Section (.data):"
objdump -h $DEMO_BIN | grep "\.data"
echo ""

echo "BSS Section (.bss):"
objdump -h $DEMO_BIN | grep "\.bss"
echo ""

echo "6. Generating Memory Layout Diagram..."
echo "------------------------------------------------------------"
echo "Memory Layout (approximate):"
echo ""
echo "High Address"
echo "    ↓"
echo "  [Stack]         (Local variables, parameters)"
echo "    ↓"
echo "  [Heap]          (Dynamic allocation)"
echo "    ↓"
echo "  [BSS]           (Uninitialized globals/statics)"
echo "    ↓"
echo "  [Data]          (Initialized globals/statics)"
echo "    ↓"
echo "  [Text/Code]     (Program instructions)"
echo "    ↓"
echo "Low Address"
echo ""

echo "=========================================="
echo "Analysis Complete!"
echo "=========================================="
echo ""
echo "Files generated:"
echo "  - $ASM_FILE (full disassembly)"
echo ""
echo "To view the full disassembly:"
echo "  cat $ASM_FILE | less"
echo ""

