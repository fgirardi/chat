#include <iostream>
#include <unordered_set>

#include <signal.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "chat.h"
#include "server.h"

/*
TODO:
* Build an keep alive to close the socket from dead clients
* Convert chat_message into a class
*/

/* file descriptors of sockets */
pthread_mutex_t messages_lock;

#ifdef CHAT_VERBOSE
void do_verbose(std::string msg)
{
	std::cout << msg << std::endl;
}

#else
void do_verbose(std::string) {}
#endif

/* Just to use in pthread_create*/

struct pthread_hack {
	int sock_client;
	std::unordered_set<int> *client_fds;
};

void send_message_to_clients(std::string msg, std::unordered_set<int> *client_fds)
{
	pthread_mutex_lock(&messages_lock);

	struct chat_message cm;
	cm.type = SERVER_MESSAGE;
	strncpy(cm.msg, msg.c_str(), msg.size() + 1);

	for (const int fd : *client_fds)
	{
		do_verbose("server: send message to socket " + std::to_string(fd));
		send(fd, &cm, sizeof(cm), 0);
	}

	pthread_mutex_unlock(&messages_lock);
}

void *recv_messages(void *ph)
{
	struct chat_message cm;
	struct pthread_hack lph = *(struct pthread_hack *)ph;
	int sockfd = lph.sock_client;
	while (1)
	{
		int size = recv(sockfd, &cm, sizeof(cm), 0);

		// in case of socket error, remove the socket from the client_fds
		if (size <= 0)
			break;

		do_verbose("server: Received msg user " + std::string(cm.nickname) +
				+ " socket " + std::to_string(sockfd)
				+ " size " + std::to_string(size)
				+ ": " + std::string(cm.msg));

		if (cm.type == SEND_MESSAGE && size > 0)
		{
			time_t t = time(NULL);
			struct tm *tm = localtime(&t);

			send_message_to_clients(std::to_string(tm->tm_mday) + "/" +
						std::to_string(tm->tm_mon + 1) + "/" + 
						std::to_string(tm->tm_year + 1900) + " " +
						std::to_string(tm->tm_hour) + ":" +
						std::to_string(tm->tm_min) + ":" +
						std::to_string(tm->tm_sec) +
						"[" + cm.nickname + "]: " + cm.msg, lph.client_fds);
		}
	}

	do_verbose("server: socket " + std::to_string(sockfd) + " closed. Removing from clients list");

	pthread_mutex_lock(&messages_lock);
	lph.client_fds->erase(sockfd);
	pthread_mutex_unlock(&messages_lock);

	int retVal = 0;

	pthread_exit(&retVal);

	return NULL;
}

Server::Server()
	: sock_server(0)
{}

Server::~Server()
{
	if (sock_server)
		close(sock_server);

	std::cout << "Server socket closed" << std::endl;
}

bool Server::init()
{
	sock_server = socket(PF_INET, SOCK_STREAM, 0);

	if (sock_server == -1)
		return false;

	bzero(&server, sizeof(server));
	server.sin_family = PF_INET;
	server.sin_port = htons(CHAT_PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock_server, (struct sockaddr *)&server, sizeof(server)))
		return false;

	if (listen(sock_server, 5))
		return false;

	return true;
}

bool Server::getClientMessages()
{
	struct sockaddr_in client;

	socklen_t client_len = sizeof(client);
	sock_client = accept(sock_server, (struct sockaddr *)&client, &client_len);

	struct chat_message cm;
	recv(sock_client, &cm, sizeof(cm), 0);

	do_verbose("server: Connection accept socked " + std::to_string(sock_client));

	if (cm.type == REGISTER)
	{
		char msg[5];
		sprintf(msg, "%s", CHAT_OK);
		send(sock_client, &msg, sizeof(msg), 0); 

		if (!strcmp(msg, CHAT_OK))
		{
			do_verbose("server: Connection successful socket " + std::to_string(sock_client));

			// protect fds create with lock
			pthread_mutex_lock(&messages_lock);
			client_fds.insert(sock_client);
			pthread_mutex_unlock(&messages_lock);

			struct pthread_hack ph = {.sock_client = sock_client, .client_fds = &client_fds};

			pthread_t pt;
			pthread_create(&pt, NULL, &recv_messages, &ph);
		}
	}

	return true;
}
