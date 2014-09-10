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
#include "server.h"

int main()
{
	Server server;

	if (!server.init())
		return 1;

	while (server.getClientMessages()) {}

	return 0;
}
