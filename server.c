#include "server.h"

int main()
{
	Server server;

	if (!server.init())
		return 1;

	while (server.getClientMessages()) {}

	return 0;
}
