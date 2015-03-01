bin_PROGRAMS = server client

AM_CXXFLAGS = -std=c++0x -Werror -Wall

server_SOURCES = server.cxx server.h screen.cxx screen.h chat.h
server_LDFLAGS = -lncurses -lpthread

client_SOURCES = client.cxx client.h screen.cxx screen.h chat.h
client_LDFLAGS = -lncurses -lpthread
