#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <mutex>
#include <unordered_set>

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
	Server();
	~Server();
	bool init();
	void finish();
	void handleMessages();
	void send_message_to_clients(std::string msg);
	void add_client(int sock_client, char *nickname);
	void remove_client(int sock_client, char *nickname);
private:
	int sock_server;
	int sock_client;
	struct sockaddr_in server;
	std::unordered_set<ClientConn, Hash> clients;
	std::mutex client_mutex;
};
