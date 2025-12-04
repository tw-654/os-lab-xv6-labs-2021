CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = test_memory_manager
SOURCES = memory_manager.c test.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c memory_manager.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

help:
	@echo "Available targets:"
	@echo "  all    - Build the test program (default)"
	@echo "  run    - Build and run the test program"
	@echo "  clean  - Remove build artifacts"
	@echo "  help   - Show this help message"

