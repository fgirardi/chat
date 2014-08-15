#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <ncurses.h>

WINDOW *create_win(int h, int w, int starty, int startx);

void add_message(char *msg);

void init_screen();
void end_screen();

#endif //__SCREEN_H__
