#include "chat.h"
#include "client.h"
#include "screen.h"

#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <thread>
#include <vector>

Client::Client(std::string addr, std::string nick)
{
	sock_server = 0;
	bzero(&server_addr, sizeof(server_addr));
	address = addr;
	nickname = nick;
	connected = false;
}

Client::~Client()
{
	if (sock_server)
		close(sock_server);
}

void Client::server_connect()
{
	sock_server = socket(PF_INET, SOCK_STREAM, 0);

	if (sock_server == -1)
		return;

	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(CHAT_PORT);
	inet_aton(address.c_str(), &server_addr.sin_addr);

	if (connect(sock_server, (struct sockaddr *)&server_addr,
					(socklen_t)sizeof(server_addr)))
		return;

	struct chat_message cm = {.type = REGISTER };
	strcpy(cm.nickname, nickname.c_str());

	if (send(sock_server, &cm, sizeof(cm), 0) == -1)
		return;

	connected = true;
}

void Client::send_user_message()
{
	while (true) {
		struct chat_message cm = {.type = SEND_MESSAGE };
		strcpy(cm.nickname, nickname.c_str());

		std::string nmsg = get_user_input();

		// avoid sending empty strings to server
		if (nmsg.empty())
			continue;

		if (!connected) {
			add_message("Server is offline...");
			continue;
		}

		strcpy(cm.msg, nmsg.c_str());
		cm.msg[strlen(cm.msg)] = '\0';

		add_message_to_window(std::string(cm.msg), true);

		if (send(sock_server, &cm, sizeof(cm), 0) == -1)
			std::cout << "Error while sending message to server...";
	}
}

void Client::recv_msgs()
{
	struct chat_message cm;

	while (1)
	{
		while (!connected) {
			add_message("Server is offline. Trying to connect in 2 two seconds");
			sleep(2);
			server_connect();

			if (connected)
				add_message("Reconnected to server");
		}

		/* stop on error or server closes the socket */
		if (recv(sock_server, &cm, sizeof(cm), 0) <= 0) {
			connected = false;
			continue;
		}

		if (cm.type == SERVER_MESSAGE)
			add_message_to_window(std::string(cm.msg));
	}
}

void Client::add_message_to_window(std::string msg, bool add_nickname)
{
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	std::string nmsg = std::to_string(tm->tm_mday) + "/" +
			std::to_string(tm->tm_mon + 1) + "/" +
			std::to_string(tm->tm_year + 1900) + " " +
			std::to_string(tm->tm_hour) + ":" +
			std::to_string(tm->tm_min) + ":" +
			std::to_string(tm->tm_sec) + " ";

	if (add_nickname)
		nmsg += "[" + nickname + "]: ";

	nmsg += msg;

	add_message(nmsg);
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

	Client client(argv[1], argv[2]);

	init_screen();

	// first attempt to connect
	client.server_connect();
	if (client.isConnected())
		add_message("Connected to server");

	std::thread recv_msgs(&Client::recv_msgs, &client);

	client.send_user_message();

	recv_msgs.join();

	end_screen();

	return 0;
}
