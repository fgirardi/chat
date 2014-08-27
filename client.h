#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

#include "chat.h"

class Client
{
	Client();
	bool server_connect(std::string);
	bool send_register_message();
	bool send_user_message(struct chat_message cm);

	int get_server_sock { return sock_server; }
private:
	int sock_server;
	struct sockaddr_in server_addr;
};
