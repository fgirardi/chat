CC=gcc
CFLAGS=-Wall -Werror -Wextra -g -lpthread

all:
	$(CC) server.c $(CFLAGS) -o server
	$(CC) client.c $(CFLAGS) -o client

clean:
	rm server client
