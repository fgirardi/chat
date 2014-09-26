#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <iostream>
#include <ncurses.h>
#include <vector>

WINDOW *create_win(int h, int w, int starty, int startx, int box);

void add_message(std::string msg);

void init_screen();
void end_screen();

std::string get_user_input();

#endif //__SCREEN_H__
