#include "screen.h"

WINDOW *messages_window = NULL;
WINDOW *user_data_window = NULL;

WINDOW *create_win(int h, int w, int starty, int startx, int box)
{
	WINDOW *win;

	win = newwin(h, w, starty, startx);
	if (box)
		box(win, 0, 0);
	wrefresh(win);

	return win;
}

void add_message(char *msg)
{
	wprintw(messages_window, "%s\n", msg);
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

void get_user_input(char *user_data)
{
	mvwscanw(user_data_window, 1, 1, "%s", user_data);
}
