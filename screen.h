#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <iostream>
#include <ncurses.h>
#include <vector>

WINDOW *create_win(int h, int w, int starty, int startx, int box);

void add_message(std::string msg);

void init_screen();
void end_screen();

void get_user_input(std::vector<char> &user_data);

#endif //__SCREEN_H__
