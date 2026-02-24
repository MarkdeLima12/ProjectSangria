# Compiler and flags
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
INCLUDES = -Iheaders

# Automatically gather all .c files
SRC = $(wildcard *.c) $(wildcard src/*.c)

# Convert .c → .o
OBJ = $(SRC:.c=.o)

# Output executable name
TARGET = main

# Default rule
all: $(TARGET)

# Link step
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Compile step
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Cleanup
clean:
	rm -f $(OBJ) $(TARGET)

# Convenience
run: $(TARGET)
	./$(TARGET)

memcheck: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)
