CC=g++
#CFLAGS=-Wall -Werror -Wextra -g -lpthread
CXXFLAGS=-Wall -Wextra -g -lpthread


all:
	$(CC) server.c $(CXXFLAGS) -o server
	$(CC) client.c client.cxx screen.c $(CXXFLAGS) -o client -lncurses

clean:
	rm server client
