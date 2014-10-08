#include <iostream>
#include <thread>
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

void Server::send_message_to_clients(std::string msg)
{
	client_mutex.lock();

	struct chat_message cm;
	cm.type = SERVER_MESSAGE;
	strncpy(cm.msg, msg.c_str(), msg.size() + 1);

	for (auto c : clients)
	{
		do_verbose("server: send message to socket " + std::to_string(c.sockid));
		send(c.sockid, &cm, sizeof(cm), 0);
	}

	client_mutex.unlock();
}

void Server::recv_messages(int sockfd)
{
	struct chat_message cm;
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
			send_message_to_clients("[" + std::string(cm.nickname) + "]: " + std::string(cm.msg));
	}

	// just to remove the sockfd from clients
	ClientConn c(sockfd, "");
	remove_client(c);
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

void Server::add_client(ClientConn &c)
{
	do_verbose("server: Connection successful socket " + std::to_string(sock_client));
	send_message_to_clients("User " + std::string(c.nickname) + " entered in the room");

	client_mutex.lock();
	clients.insert(c);
	client_mutex.unlock();
}

void Server::remove_client(ClientConn &cli)
{
	client_mutex.lock();
	auto c = clients.find(cli);

	if (c != clients.end()) {
		do_verbose("server: socket " + std::to_string(c->sockid) + " closed. Removing from clients list");
		send_message_to_clients("User " + std::string(c->nickname) + " was disconnected from the room");
	}

	clients.erase(cli);
	client_mutex.unlock();
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

int Server::getClientMessages()
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
			ClientConn c(sock_client, cm.nickname);

			add_client(c);

			return sock_client;
		}
	}

	return 0;
}

int main()
{
	Server server;

	if (!server.init())
		return 1;

	while (true) {
		int sock_client = server.getClientMessages();
		if (sock_client == 0)
			break;

		std::thread t(&Server::recv_messages, &server, sock_client);
		t.detach();
	}
	return 0;
}