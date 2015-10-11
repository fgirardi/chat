#ifndef __CHAT_H__
#define __CHAT_H__

#define CHAT_PORT 9090

struct chat_message {
	char nickname[15];
	char msg[100];
};

#ifdef ENABLE_VERBOSE
void do_verbose(std::string msg)
{
	add_message(msg);
}
#else
void do_verbose(std::string) {}
#endif

#endif //__CHAT_H__
