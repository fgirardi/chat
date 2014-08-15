#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "chat.h"

/*
TODO:
* Build an keep alive to close the socket from dead clients
* Build an minimal UI using ncurses to better present the
	messages from users
*/

/* file descriptors of sockets */
int sock_server, sock_client;

pthread_mutex_t messages_lock;

struct registered_nodes registered;
pthread_t threads[MAX_CONN];

/* verbose flag */
int verbose = 0;
char verbosity[100];

void do_verbose(char *msg)
{
	if (verbose)
		printf("%s", msg);
}

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

void send_message_to_clients(char *msg)
{
	pthread_mutex_lock(&messages_lock);

	int i;
	struct chat_message cm;
	cm.type = SERVER_MESSAGE;
	strncpy(cm.msg, msg, sizeof(cm.msg));
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
		snprintf(verbosity, sizeof(verbosity), "server: Received msg user %s socket %d size %d: %s"
							, cm.nickname, sockfd, size, cm.msg);
		do_verbose(verbosity);
		if (cm.type == SEND_MESSAGE && size > 0)
		{
			time_t t = time(NULL);
			struct tm *tm = localtime(&t);

			char msg[200];
			snprintf(msg, sizeof(msg), "%02d/%02d/%02d %02d:%02d:%02d [%s]: %s", tm->tm_mday, tm->tm_mon + 1,
										tm->tm_year + 1900, tm->tm_hour, tm->tm_min,
										tm->tm_sec, cm.nickname, cm.msg);
			send_message_to_clients(msg);
		}
	}
	return NULL;
}

void finish_server()
{
	close(sock_server);
	printf("Closed the servers socket\n");

	exit(0);
}

void sighandler(int signum, siginfo_t *info, void *ptr)
{
	(void)signum;
	(void)info;
	(void)ptr;

	finish_server();
}

int main(int argc, char *argv[])
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

	if (argc > 1 && atoi(argv[1]) == 1)
		verbose = 1;

	while (1)
	{
		socklen_t size_client = sizeof(client);
		sock_client = accept(sock_server, (struct sockaddr *)&client, &size_client);

		struct chat_message cm;
		recv(sock_client, &cm, sizeof(cm), 0);

		snprintf(verbosity, sizeof(verbosity),"server: Connection accept socked %d type %d\n", sock_client, cm.type);
		do_verbose(verbosity);

		if (cm.type == REGISTER)
		{
			char msg[5];
			if (added_registered_node(sock_client))
				sprintf(msg, "%s", NOK);
			else
				sprintf(msg, "%s", OK);
			send(sock_client, &msg, sizeof(msg), 0);

			if (!strcmp(msg, OK))
			{
				snprintf(verbosity, sizeof(verbosity), "server: Connection successful socket %d\n", sock_client);
				do_verbose(verbosity);
				pthread_create(&threads[registered.how -1], NULL, &recv_messages, (void *)&sock_client);
			}
		}
	}

	finish_server();

	return 0;
}
