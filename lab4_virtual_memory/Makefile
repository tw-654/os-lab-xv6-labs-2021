CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = test_virtual_memory
SOURCES = virtual_memory.c test.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean run help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c virtual_memory.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

help:
	@echo "Available targets:"
	@echo "  all    - Build the virtual memory system test program (default)"
	@echo "  run    - Build and run the test program"
	@echo "  clean  - Remove build artifacts"
	@echo "  help   - Show this help message"

