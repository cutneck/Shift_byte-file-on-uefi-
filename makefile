TARGET = shift_file

CC = gcc
CFLAGS = -Wall -Wextra -O2

SRCS = shift_file.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

clean_objs:
	rm -f $(OBJS)

.PHONY: all clean clean_objs
