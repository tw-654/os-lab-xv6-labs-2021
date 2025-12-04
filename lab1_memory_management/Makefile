CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = test_memory_manager
TARGET_ADV = test_advanced
SOURCES = memory_manager.c test.c
SOURCES_ADV = memory_manager.c test_advanced.c
OBJECTS = $(SOURCES:.c=.o)
OBJECTS_ADV = $(SOURCES_ADV:.c=.o)

.PHONY: all clean run run-adv help

all: $(TARGET)

advanced: $(TARGET_ADV)

$(TARGET): memory_manager.o test.o
	$(CC) $(CFLAGS) -o $(TARGET) memory_manager.o test.o

$(TARGET_ADV): memory_manager.o test_advanced.o
	$(CC) $(CFLAGS) -o $(TARGET_ADV) memory_manager.o test_advanced.o

%.o: %.c memory_manager.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

run-adv: $(TARGET_ADV)
	./$(TARGET_ADV)

clean:
	rm -f $(OBJECTS) $(OBJECTS_ADV) $(TARGET) $(TARGET_ADV)

help:
	@echo "Available targets:"
	@echo "  all       - Build the basic test program (default)"
	@echo "  advanced  - Build the advanced test program"
	@echo "  run       - Build and run the basic test program"
	@echo "  run-adv   - Build and run the advanced test program"
	@echo "  clean     - Remove build artifacts"
	@echo "  help      - Show this help message"

