CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: printing_scheduler

printing_scheduler: main.c
	$(CC) $(CFLAGS) -o printing_scheduler main.c

clean:
	rm -f printing_scheduler
