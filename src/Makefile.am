bin_PROGRAMS = server client

server_SOURCES = server.cxx server.h chat.h
server_LDFLAGS = -lpthread
server_CXXFLAGS = -std=c++0x -Werror -Wall

client_SOURCES = client.cxx client.h screen.cxx screen.h chat.h
client_LDFLAGS = -lncurses -lpthread
client_CXXFLAGS = -std=c++0x -Werror -Wall
