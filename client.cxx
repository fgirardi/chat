#include "chat.h"
#include "client.h"
#include "screen.h"

#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <vector>

Client::Client(std::string nick)
{
	sock_server = 0;
	bzero(&server_addr, sizeof(server_addr));
	nickname = nick;
}

Client::~Client()
{
	if (sock_server)
		close(sock_server);
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

bool Client::send_register_message()
{
	struct chat_message cm = {.type = REGISTER };

	if (send(sock_server, &cm, sizeof(cm), 0) == -1)
		return false;

	char ret[100];

	if (recv(sock_server, &ret, sizeof(ret), 0) == -1)
		return false;

	if (!strcmp(ret, CHAT_OK))
		return true;

	return false;
}

bool Client::send_user_message()
{
	struct chat_message cm = {.type = SEND_MESSAGE };
	strcpy(cm.nickname, nickname.c_str());

	std::vector<char> msg(100);
	get_user_input(msg);

	std::string nmsg(msg.begin(), msg.end());

	strcpy(cm.msg, nmsg.c_str());
	cm.msg[strlen(cm.msg)] = '\0';

	if (send(sock_server, &cm, sizeof(cm), 0) == -1) {
		std::cout << "Error while sending message to server. Aborting...\n";
		return false;
	}

	return true;
}
