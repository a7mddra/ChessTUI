
CC = gcc
CFLAGS = -Wall -g

SRCS = main.c lib/colors.c
OBJS = main.o lib/colors.o

TARGET = chess_game

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)