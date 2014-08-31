#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

#include "chat.h"

class Client
{
public:
	Client(std::string nickname);
	~Client();
	bool server_connect(std::string);
	bool send_register_message();
	bool send_user_message();
	void recv_msgs();

private:
	int sock_server;
	struct sockaddr_in server_addr;
	std::string nickname;
};
