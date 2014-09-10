#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <unordered_set>

class Server
{
public:
	Server();
	~Server();
	bool init();
	void finish();
	bool getClientMessages();
private:
	int sock_server;
	int sock_client;
	struct sockaddr_in server;
	std::unordered_set<int> client_fds;
};
