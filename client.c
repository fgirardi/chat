#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "chat.h"
#include "client.h"
#include "screen.h"

// hold the nickname by parameter
char *nickname;

void helpMessage()
{
	std::cout << "You need to give the address number and nickname as argument!";
	exit(1);
}

void *receive_message_from_users(void *sock_server)
{
	struct chat_message cm;
	int *sockfd = (int*)sock_server;

	while (1)
	{
		/* stop on error or server closes the socket */
		if (recv(*sockfd, &cm, sizeof(cm), 0) == -1) {
			std::cout << "Error while trying to receive message from server. Aborting...\n";
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

	struct sockaddr_in server;
	int sock_server;

	sock_server = socket(PF_INET, SOCK_STREAM, 0);

	bzero(&server, sizeof(server));

	server.sin_family = PF_INET;
	server.sin_port = htons(CHAT_PORT);
	inet_aton(argv[1], &server.sin_addr);

	int ret = connect(sock_server, (struct sockaddr *)&server, (socklen_t)sizeof(server));

	if (ret)
		perror("Can't connect to server");
	else
	{
		init_screen();

		struct chat_message cm;
		cm.type = REGISTER;
		strncpy(cm.nickname, argv[2], sizeof(cm.nickname));

		send(sock_server, &cm, sizeof(cm), 0);

		char ret[100];
		recv(sock_server, &ret, sizeof(ret), 0);

		if (!strcmp(ret, CHAT_OK))
		{
			add_message("Received OK from server...");

			pthread_t thread_id;
			pthread_create(&thread_id, NULL, &receive_message_from_users, &sock_server);
			cm.type = SEND_MESSAGE;

			while (1)
			{
				//fgets(cm.msg, sizeof(cm.msg), stdin);
				std::vector<char> msg(100);
				get_user_input(msg);

				std::string nmsg(msg.begin(), msg.end());

				strcpy(cm.msg, nmsg.c_str());
				cm.msg[strlen(cm.msg)] = '\0';

				if (send(sock_server, &cm, sizeof(cm), 0) == -1) {
					std::cout << "Error while sending message to server. Aborting...\n";
					break;
				}
			}
		}

		end_screen();
	}

	close(sock_server);

	return 0;
}
