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
TARGET2 = main2

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
	rm -f $(OBJ) $(TARGET) $(TARGET2)

# Convenience
run: $(TARGET)
	./$(TARGET)

$(TARGET2):
	$(CC) $(CFLAGS) $(INCLUDES) -Dmain2=main main2.c src/*.c -o $(TARGET2)

run2: $(TARGET2)
	./$(TARGET2)

memcheck: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)
