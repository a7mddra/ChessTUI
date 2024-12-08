
CC = gcc
CFLAGS = -Wall -g

SRCS = main.c terminal/termios.c terminal/colors.c
OBJS = main.o terminal/termios.o terminal/colors.o

TARGET = chess_game

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)