#include <iostream>
#include <thread>
#include <unordered_set>

#include <signal.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <fcntl.h>

#include "chat.h"
#include "server.h"

/*
TODO:
* Convert chat_message into a class
*/

#define MAX_EVENTS 100

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

void Server::add_client(int sock_client, char* nickname)
{
	ClientConn c(sock_client, nickname);
	do_verbose("server: Connection successful socket " + std::to_string(sock_client));
	send_message_to_clients("User " + std::string(c.nickname) + " entered in the room");

	client_mutex.lock();
	clients.insert(c);
	client_mutex.unlock();
}

void Server::remove_client(int sock_client, char *nickname)
{
	ClientConn cli(sock_client, nickname);

	auto c = clients.find(cli);

	if (c != clients.end()) {
		do_verbose("server: socket " + std::to_string(c->sockid) + " closed. Removing from clients list");
		send_message_to_clients("User " + std::string(c->nickname) + " was disconnected from the room");
	}

	client_mutex.lock();
	clients.erase(cli);
	client_mutex.unlock();

	// close client socket
	close(sock_client);
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

void Server::handleMessages()
{
	struct epoll_event ev, events[MAX_EVENTS];

	int epollfd = epoll_create1(0);
	if (epollfd == -1) {
		do_verbose("epoll_create error");
		return;
	}

	ev.events = EPOLLIN;
	ev.data.fd = sock_server;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_server, &ev) == -1) {
		do_verbose("epoll_ctl: listen socket");
		return;
	}

	while (true) {
		int n = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (n == -1) {
			do_verbose("epoll_wait");
			return;
		}

		for (int i = 0; i < n; i++) {
			// handle new connections into server
			if (events[i].data.fd == sock_server) {
				struct sockaddr_in client;

				socklen_t client_len = sizeof(client);
				sock_client = accept(sock_server, (struct sockaddr *)&client, &client_len);

				if (sock_client == -1) {
					do_verbose("accept");
					return;
				}

				//setnonblocking
				int flags = fcntl(sock_client, F_GETFL, 0);
				fcntl(sock_client, F_SETFL, flags | O_NONBLOCK);

				do_verbose("server: Connection accept socked " + std::to_string(sock_client));

				ev.events = EPOLLIN | EPOLLOUT;
				ev.data.fd = sock_client;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_client, &ev) == -1) {
					do_verbose("epoll_ctl: sock client");
					return;
				}
			} else {
				// handle existing connections
				struct chat_message cm;

				// get the client fd
				sock_client = events[i].data.fd;

				int n = recv(sock_client, &cm, sizeof(cm), 0);

				if (n == 0) {
					remove_client(sock_client, cm.nickname);
					continue;
				}

				// we don't have data on this fd, skip for now
				if (n < 0 && errno == EAGAIN)
					continue;

				if (cm.type == REGISTER)
				{
					add_client(sock_client, cm.nickname);

				} else if (cm.type == SEND_MESSAGE) {
					do_verbose("server: Received msg user " + std::string(cm.nickname) +
							+ " socket " + std::to_string(sock_client)
							+ " size " + std::to_string(n)
							+ ": " + std::string(cm.msg));

					send_message_to_clients("[" + std::string(cm.nickname) + "]: " + std::string(cm.msg));
				}
			}
		}
	}
}

int main()
{
	Server server;

	if (!server.init())
		return 1;

	server.handleMessages();

	return 0;
}
