#include "chat.h"
#include "client.h"
#include "screen.h"

#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <thread>
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
	strcpy(cm.nickname, nickname.c_str());

	if (send(sock_server, &cm, sizeof(cm), 0) == -1)
		return false;

	char ret[100];

	if (recv(sock_server, &ret, sizeof(ret), 0) <= 0)
		return false;

	if (!strcmp(ret, CHAT_OK))
		return true;

	return false;
}

bool Client::send_user_message()
{
	struct chat_message cm = {.type = SEND_MESSAGE };
	strcpy(cm.nickname, nickname.c_str());

	std::string nmsg = get_user_input();

	// avoid sending empty strings to server
	if (nmsg.empty())
		return true;

	strcpy(cm.msg, nmsg.c_str());
	cm.msg[strlen(cm.msg)] = '\0';

	if (send(sock_server, &cm, sizeof(cm), 0) == -1) {
		std::cout << "Error while sending message to server. Aborting...\n";
		return false;
	}

	return true;
}

void Client::recv_msgs()
{
	struct chat_message cm;

	while (1)
	{
		/* stop on error or server closes the socket */
		if (recv(sock_server, &cm, sizeof(cm), 0) <= 0) {
			std::cout << "Error while trying to receive message from server. Aborting..." << std::endl;
			break;
		}

		if (cm.type == SERVER_MESSAGE)
		{
			time_t t = time(NULL);
			struct tm *tm = localtime(&t);

			std::string msg = std::to_string(tm->tm_mday) + "/" +
						std::to_string(tm->tm_mon + 1) + "/" +
						std::to_string(tm->tm_year + 1900) + " " +
						std::to_string(tm->tm_hour) + ":" +
						std::to_string(tm->tm_min) + ":" +
						std::to_string(tm->tm_sec) + " " + cm.msg;

			add_message(msg);
		}
	}
}

void Client::helpMessage()
{
	std::cout << "You need to give the address number and nickname as argument!" << std::endl;
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc < 3)
		Client::helpMessage();

	Client client(argv[2]);

	if (!client.server_connect(argv[1])) {
		perror("Can't connect to server");
		return 1;
	}

	init_screen();

	if (client.send_register_message())
	{
		add_message("Received OK from server...");

		std::thread recv_msgs(&Client::recv_msgs, &client);

		while (1)
			if (!client.send_user_message())
				break;

		recv_msgs.join();
	}

	end_screen();

	return 0;
}
