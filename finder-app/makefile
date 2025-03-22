# Compiler and Linker
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
LD := $(CROSS_COMPILE)ld

# Flags
CFLAGS := -Wall -Werror
LDFLAGS :=

# Source files and object files
SRC := writer.c
OBJ := $(SRC:.c=.o)

# Target executable
TARGET := writer

# Default target (build the writer application)
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Rule to build object files
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# Clean target to remove build artifacts
clean:
	rm -f $(TARGET) $(OBJ)

# Phony targets (not files)
.PHONY: all clean