CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -fno-omit-frame-pointer
TARGET_DEMO = func_demo
TARGET_ANALYSIS = memory_analysis
SOURCES_DEMO = func_demo.c
SOURCES_ANALYSIS = memory_analysis.c
OBJECTS_DEMO = $(SOURCES_DEMO:.c=.o)
OBJECTS_ANALYSIS = $(SOURCES_ANALYSIS:.c=.o)

.PHONY: all clean run demo analysis analyze gdb help

all: $(TARGET_DEMO) $(TARGET_ANALYSIS)

$(TARGET_DEMO): $(OBJECTS_DEMO)
	$(CC) $(CFLAGS) -o $(TARGET_DEMO) $(OBJECTS_DEMO)

$(TARGET_ANALYSIS): $(OBJECTS_ANALYSIS)
	$(CC) $(CFLAGS) -o $(TARGET_ANALYSIS) $(OBJECTS_ANALYSIS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: demo

demo: $(TARGET_DEMO)
	./$(TARGET_DEMO)

analysis: $(TARGET_ANALYSIS)
	./$(TARGET_ANALYSIS)

analyze: $(TARGET_DEMO)
	@chmod +x analyze.sh
	./analyze.sh

gdb: $(TARGET_DEMO)
	gdb -batch -x gdb_analysis.gdb $(TARGET_DEMO)

disasm: $(TARGET_DEMO)
	@echo "=========================================="
	@echo "Disassembly of func_demo"
	@echo "=========================================="
	@objdump -d $(TARGET_DEMO)
	@echo ""
	@echo "Saving to func_demo.asm..."
	@objdump -d $(TARGET_DEMO) > func_demo.asm

clean:
	rm -f $(OBJECTS_DEMO) $(OBJECTS_ANALYSIS) $(TARGET_DEMO) $(TARGET_ANALYSIS)
	rm -f func_demo.asm func_demo.map
	rm -f *.o

help:
	@echo "Available targets:"
	@echo "  all        - Build both demo and analysis programs (default)"
	@echo "  demo       - Build and run the demo program"
	@echo "  analysis   - Build and run the memory analysis program"
	@echo "  analyze    - Run comprehensive analysis script (requires objdump/readelf)"
	@echo "  gdb        - Run GDB analysis script"
	@echo "  disasm     - Generate disassembly file"
	@echo "  clean      - Remove build artifacts"
	@echo "  help       - Show this help message"

