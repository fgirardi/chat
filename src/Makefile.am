bin_PROGRAMS = server client

nobase_include_HEADERS = chat.h server.h client.h screen.h

server_SOURCES = server.cxx
server_LDFLAGS = -lpthread
server_CXXFLAGS = -std=c++0x

client_SOURCES = client.cxx screen.cxx
client_LDFLAGS = -lncurses -lpthread
client_CXXFLAGS = -std=c++0x
