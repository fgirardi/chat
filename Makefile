CC=g++
CXXFLAGS=-Wall -Wextra -g -lpthread -std=c++0x

# just use in case of debug
CXXFLAGS += -DCHAT_VERBOSE

all:
	$(CC) server.c server.cxx $(CXXFLAGS) -o server
	$(CC) client.c client.cxx screen.c $(CXXFLAGS) -o client -lncurses

clean:
	rm server client
