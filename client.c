#include <iostream>
#include <thread>

#include "chat.h"
#include "client.h"
#include "screen.h"

void helpMessage()
{
	std::cout << "You need to give the address number and nickname as argument!" << std::endl;
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc < 3)
		helpMessage();

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
