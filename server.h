#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <unordered_set>

class ClientConn
{
public:
	ClientConn(int sockid, std::string name);

	bool operator==(const ClientConn &c) const {
		return c.sockid == sockid;
	}

	int sockid;
private:
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
	bool getClientMessages();
	void notifyNewClient(std::string);
private:
	int sock_server;
	int sock_client;
	struct sockaddr_in server;
	std::unordered_set<ClientConn, Hash> clients;
};
