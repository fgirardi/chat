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
	std::unordered_set<ClientConn, Hash> *clients;
};

void send_message_to_clients(std::string msg, std::unordered_set<ClientConn, Hash> *clients)
{
	pthread_mutex_lock(&messages_lock);

	struct chat_message cm;
	cm.type = SERVER_MESSAGE;
	strncpy(cm.msg, msg.c_str(), msg.size() + 1);

	for (auto c : *clients)
	{
		do_verbose("server: send message to socket " + std::to_string(c.sockid));
		send(c.sockid, &cm, sizeof(cm), 0);
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

		// in case of socket error, remove the socket from the clients
		if (size <= 0)
			break;

		do_verbose("server: Received msg user " + std::string(cm.nickname) +
				+ " socket " + std::to_string(sockfd)
				+ " size " + std::to_string(size)
				+ ": " + std::string(cm.msg));

		if (cm.type == SEND_MESSAGE && size > 0)
			send_message_to_clients("[" + std::string(cm.nickname) + "]: " + std::string(cm.msg), lph.clients);
	}

	do_verbose("server: socket " + std::to_string(sockfd) + " closed. Removing from clients list");

	// just to remove the sockfd from clients
	ClientConn c(sockfd, "");

	auto cdata = lph.clients->find(c);

	if (cdata != lph.clients->end())
		send_message_to_clients("User " + std::string(cdata->nickname) + " was disconnected from the room", lph.clients);

	pthread_mutex_lock(&messages_lock);
	lph.clients->erase(c);
	pthread_mutex_unlock(&messages_lock);

	int retVal = 0;

	pthread_exit(&retVal);

	return NULL;
}

ClientConn::ClientConn(int sockid, std::string name)
	: sockid(sockid)
	, nickname(name)
{}

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

			ClientConn c(sock_client, cm.nickname);

			// protect fds create with lock
			pthread_mutex_lock(&messages_lock);
			clients.insert(c);
			pthread_mutex_unlock(&messages_lock);

			send_message_to_clients("User " + std::string(c.nickname) + " entered in the room", &clients);


			struct pthread_hack ph = {.sock_client = sock_client, .clients = &clients};

			pthread_t pt;
			pthread_create(&pt, NULL, &recv_messages, &ph);
		}
	}

	return true;
}

int main()
{
	Server server;

	if (!server.init())
		return 1;

	while (server.getClientMessages()) {}

	return 0;
}
