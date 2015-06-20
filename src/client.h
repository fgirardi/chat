#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

class Client
{
public:
	~Client();
	void server_connect();
	void send_user_message();
	void add_message_to_window(std::string msg, bool add_nickname = false);
	void recv_msgs();
	bool isConnected() { return connected; }
	static void helpMessage();

	void initClient(std::string addr, std::string nickname);

	// singleton
	static Client* getInstance();

private:
	// singleton
	Client() {};
	Client (Client const&);
	Client&  operator = (const Client& c);

	bool connected;
	int sock_server;
	struct sockaddr_in server_addr;
	std::string address;
	std::string nickname;
};
