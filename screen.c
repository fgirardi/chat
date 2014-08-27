#include "screen.h"
#include <iostream>

WINDOW *messages_window = NULL;
WINDOW *user_data_window = NULL;

WINDOW *create_win(int h, int w, int starty, int startx, int with_box)
{
	WINDOW *win;

	win = newwin(h, w, starty, startx);
	if (with_box)
		box(win, 0, 0);
	wrefresh(win);

	return win;
}

void add_message(std::string msg)
{
	wprintw(messages_window, "%s\n", msg.c_str());
	wrefresh(messages_window);
}

void end_screen()
{
	if (messages_window)
		delwin(messages_window);
	if (user_data_window)
		delwin(user_data_window);
	endwin();
}

void init_screen()
{
	initscr();

	user_data_window = create_win(3, COLS, LINES - 3, 0, 1);
	messages_window = create_win(LINES - 3, COLS, 0, 0, 0);

	scrollok(messages_window, TRUE);
}

void get_user_input(std::vector<char> &user_data)
{
	werase(user_data_window);
	box(user_data_window, 0, 0);
	wmove(user_data_window, 1, 1);
	wrefresh(user_data_window);
	wgetnstr(user_data_window, &user_data[0], 100);
}
