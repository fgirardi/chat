CC=gcc
CFLAGS=-Wall -Werror -Wextra -g -lpthread

all:
	$(CC) server.c $(CFLAGS) -o server
	$(CC) client.c screen.c $(CFLAGS) -o client -lncurses

clean:
	rm server client
