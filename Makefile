#Make a file code
# @ file Makefile # @Buildsystem for Minesweeper
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g $(shell pkg-config --cflags notcurses)
LDFLAGS = $(shell pkg-config --libs notcurses)

TARGET = Minesweeper
SRCS = main.c board.c ui.c input.c config.c game.c variants.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS) $(TARGET)
valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)
.PHONY: all clean valgrind
