#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "chat.h"

/*
TODO:
* Build an keep alive to close the socket from dead clients
* Create server class
* Convert chat_message into a class
* Convert threads array into std::vector
*/

/* file descriptors of sockets */
int sock_server, sock_client;

pthread_mutex_t messages_lock;

struct registered_nodes registered;
pthread_t threads[MAX_CONN];

#ifdef CHAT_VERBOSE

void do_verbose(std::string msg)
{
	std::cout << msg << std::endl;
}

#else

void do_verbose(std::string) {}

#endif


void init()
{
	registered.how = 0;
	pthread_mutex_init(&messages_lock, NULL);
}

int added_registered_node(int sockfd)
{
	if (registered.how == MAX_CONN)
		return 1;
	registered.fds[registered.how++] = sockfd;
	return 0;
}

void send_message_to_clients(std::string msg)
{
	pthread_mutex_lock(&messages_lock);

	int i;
	struct chat_message cm;
	cm.type = SERVER_MESSAGE;
	strncpy(cm.msg, msg.c_str(), msg.size() + 1);
	for (i = 0; i < registered.how; i++)
		send(registered.fds[i], &cm, sizeof(cm), 0);

	pthread_mutex_unlock(&messages_lock);
}

void *recv_messages(void *sock_client)
{
	struct chat_message cm;
	int sockfd = *(int *)sock_client;
	while (1)
	{
		int size = recv(sockfd, &cm, sizeof(cm), 0);
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
						"[" + cm.nickname + "]: " + cm.msg);
		}
	}
	return NULL;
}

void finish_server()
{
	close(sock_server);
	std::cout << "Closed the servers socket" << std::endl;

	exit(0);
}

void sighandler(int signum, siginfo_t *info, void *ptr)
{
	(void)signum;
	(void)info;
	(void)ptr;

	finish_server();
}

int main()
{
	struct sockaddr_in server, client;
	struct sigaction act;

	bzero(&act, sizeof(act));

	init();

	sock_server = socket(PF_INET, SOCK_STREAM, 0);

	act.sa_sigaction = sighandler;
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGINT, &act, NULL);

	bzero(&server, sizeof(server));
	server.sin_family = PF_INET;
	server.sin_port = htons(CHAT_PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	bind(sock_server, (struct sockaddr *)&server, sizeof(server));
	listen(sock_server, 5);

	while (1)
	{
		socklen_t size_client = sizeof(client);
		sock_client = accept(sock_server, (struct sockaddr *)&client, &size_client);

		struct chat_message cm;
		recv(sock_client, &cm, sizeof(cm), 0);

		do_verbose("server: Connection accept socked " + std::to_string(sock_client));

		if (cm.type == REGISTER)
		{
			char msg[5];
			if (added_registered_node(sock_client))
				sprintf(msg, "%s", CHAT_NOK);
			else
				sprintf(msg, "%s", CHAT_OK);
			send(sock_client, &msg, sizeof(msg), 0);

			if (!strcmp(msg, CHAT_OK))
			{
				do_verbose("server: Connection successful socket " + std::to_string(sock_client));
				pthread_create(&threads[registered.how -1], NULL, &recv_messages, (void *)&sock_client);
			}
		}
	}

	finish_server();

	return 0;
}
