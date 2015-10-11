#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <map>
#include <mutex>
#include <unordered_set>
#include <vector>

class ClientConn
{
public:
	ClientConn(int sockid, std::string name);

	bool operator==(const ClientConn &c) const {
		return c.sockid == sockid;
	}

	int sockid;
	std::string nickname;
};

struct Hash
{
	size_t operator() (const ClientConn &c) const {
		return static_cast<size_t>(c.sockid);
	}
};

class Server
{
public:
	~Server();
	bool init();
	void finish();
	void handleMessages();
	void send_message_to_clients(int sock_client, std::string msg);
	void add_client(int sock_client, char *nickname);
	void check_client(int sock_client, char *nickname);
	void remove_client(int sock_client, char *nickname);
	void getUserInput();

	bool listUsers();
	bool listCommands();
	bool exitCommand();

	// singleton
	static Server* getInstance();

private:
	// singleton
	Server() {};
	Server (Server const&);
	Server&  operator = (const Server& s);

	int sock_server;
	int epollfd;
	struct sockaddr_in server;

	std::vector<std::string> m_commands;

	std::unordered_set<ClientConn, Hash> clients;
	std::mutex client_mutex;
};
