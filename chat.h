#define CHAT_PORT 5678
#define MAX_CONN 5
#define OK "OK"
#define NOK "NOK"

enum modes {
	REGISTER,
	SEND_MESSAGE,
	SERVER_MESSAGE
};

struct registered_nodes {
	int fds[MAX_CONN];
	int how;
};

struct chat_message {
	int type;
	char msg[100];
	char nickname[15];
};
