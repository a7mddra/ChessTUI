# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Source files and object files
SRCS = main.c terminal/termios.c terminal/colors.c
OBJS = main.o terminal/termios.o terminal/colors.o

# Target executable
TARGET = chess_game

# Default rule
all: $(TARGET)

# Rule to link the object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJS) $(TARGET)
