#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <ncurses.h>

WINDOW *create_win(int h, int w, int starty, int startx, int box);

void add_message(char *msg);

void init_screen();
void end_screen();

void get_user_input(char *user_data);

#endif //__SCREEN_H__
