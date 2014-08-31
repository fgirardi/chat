#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <thread>

#include "chat.h"
#include "client.h"
#include "screen.h"

void helpMessage()
{
	std::cout << "You need to give the address number and nickname as argument!" << std::endl;
	exit(1);
}

void *receive_message_from_users(void *sock_server)
{
	struct chat_message cm;
	int *sockfd = (int*)sock_server;

	while (1)
	{
		/* stop on error or server closes the socket */
		if (recv(*sockfd, &cm, sizeof(cm), 0) <= 0) {
			std::cout << "Error while trying to receive message from server. Aborting..." << std::endl;
			break;
		}

		if (cm.type == SERVER_MESSAGE)
			add_message(cm.msg);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
		helpMessage();

	Client client(argv[2]);

	if (!client.server_connect(argv[1])) {
		perror("Can't connect to server");
		return 1;
	}

	init_screen();

	if (client.send_register_message())
	{
		add_message("Received OK from server...");
		int server_sock = client.get_server_sock();

		std::thread recv_msgs(receive_message_from_users, &server_sock);

		while (1)
			if (!client.send_user_message())
				break;

		recv_msgs.join();
	}

	end_screen();

	return 0;
}
