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

void add_message(char *msg)
{
	wprintw(messages_window, "%s\n", msg);
	scroll(messages_window);
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

	user_data_window = create_win(4, COLS, LINES - 4, 0);
	messages_window = create_win(LINES - 4, COLS, 0, 0);

	scrollok(messages_window, TRUE);
}
