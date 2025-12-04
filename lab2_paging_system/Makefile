CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = test_paging
SOURCES = paging_system.c test.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean run help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c paging_system.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

help:
	@echo "Available targets:"
	@echo "  all    - Build the paging system test program (default)"
	@echo "  run    - Build and run the test program"
	@echo "  clean  - Remove build artifacts"
	@echo "  help   - Show this help message"

