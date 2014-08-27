#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

class Client
{
	Client();
	bool server_connect(std::string);
private:
	int sock_server;
	struct sockaddr_in server_addr;
};
