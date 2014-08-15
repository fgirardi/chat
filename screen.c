#include "screen.h"

WINDOW *messages_window = NULL;
WINDOW *user_data_window = NULL;

WINDOW *create_win(int h, int w, int starty, int startx)
{
	WINDOW *win;

	win = newwin(h, w, starty, startx);
	box(win, 0, 0);
	wrefresh(win);

	return win;
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
}
