CC=g++
#CFLAGS=-Wall -Werror -Wextra -g -lpthread
CFLAGS=-Wall -Wextra -g -lpthread

all:
	$(CC) server.c $(CFLAGS) -o server
	$(CC) client.c client.cxx screen.c $(CFLAGS) -o client -lncurses

clean:
	rm server client
