#include "chat.h"
#include "client.h"

#include <strings.h>

Client::Client()
{
	bzero(&server_addr, sizeof(server_addr));
}

bool Client::server_connect(std::string address)
{
	sock_server = socket(PF_INET, SOCK_STREAM, 0);

	if (sock_server == -1)
		return false;

	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(CHAT_PORT);
	inet_aton(address.c_str(), &server_addr.sin_addr);

	if (connect(sock_server, (struct sockaddr *)&server_addr,
					(socklen_t)sizeof(server_addr)))
		return false;

	return true;
}
