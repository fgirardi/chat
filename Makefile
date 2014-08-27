CC=g++
#CFLAGS=-Wall -Werror -Wextra -g -lpthread
CXXFLAGS=-Wall -Wextra -g -lpthread -std=c++11

# just use in case of debug
#CXXFLAGS += -DCHAT_VERBOSE

all:
	$(CC) server.c $(CXXFLAGS) -o server
	$(CC) client.c client.cxx screen.c $(CXXFLAGS) -o client -lncurses

clean:
	rm server client
