#ifndef __CHAT_H__
#define __CHAT_H__

#define CHAT_PORT 9090
#define CHAT_OK "OK"
#define CHAT_NOK "NOK"

enum modes {
	REGISTER,
	SEND_MESSAGE,
	SERVER_MESSAGE
};

struct chat_message {
	int type;
	char msg[100];
	char nickname[15];
};

#endif //__CHAT_H__
