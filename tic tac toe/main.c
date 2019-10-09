#include "tictactoe.h"

int main()
{
	WINDOW *board;
	char b[3][3] = {{'.', '.', '.'}, {'.', '.', '.'}, {'.', '.', '.'}};
	int status = 0;
	int who = 1;

	initscr();
	noecho();
	cbreak();
	refresh();
	while(status == 0) {
		status = turn(b, status, who, board);
		if (who == 1) {
			who++;
		} else {
			who--;
		}
	}
	print(status);
	getch();
	endwin();
	return 0;
}
