#include <ncurses.h>
#include<curses.h>
#include "tictactoe.h"

/* global varibles*/
int height=0;
int width=0;

void draw(char b[3][3], WINDOW *board)
{
	board = draw_win(10, 10, 1, 3);
	int i, j, y, x = 0;
	extern int height, width;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			y = 10 / 3 * i + 1;
			x = 10 / 3 * j + 1;
			if (i == height && j == width) {  
				wattron(board, A_REVERSE);
				mvwprintw(board, y, x, "%c ", b[i][j]);
				wattroff(board, A_REVERSE);
			} else {
				mvwprintw(board, y, x, "%c ", b[i][j]);
			}
		}
		x++;
	}
	wrefresh(board);
}

int your_turn(char b[3][3], int status, WINDOW *board)
{
	int ch;
	int bound = 0;

	extern int height, width;
	draw(b, board);

	while((ch = getch()) && bound == 0) {
		switch(ch) {
			case 'l':
				if (width > 0) {
			         	 width=width-1;
					draw(b, board);
				}
				break;
			case 'r':
				if (width < 2) {
				width=width+1;;
			       	draw(b, board);
				}
				break;
			case 'u':
				if (height > 0) {
				    height=height-1;
				    draw(b, board);
				}
				break;
			case 'd':
				if (height < 2) {
					height=height+1;
					draw(b, board);
				}
				break;
			case '.':
				if (b[height][width] == '.') {
					b[height][width] = 'X';
					draw(b, board);
					bound = 1;
				}
				break;
		}
	}	
	status = state(b);
	return status;
}

int computer_turn(char b[3][3], int status, WINDOW *board)
{
	int i, j, move = 0;
	for (i = 0; i < 3 && move == 0; i++) {
		for (j = 0; j < 3 && move == 0; j++) {
			if (b[i][j] == '.') {
				b[i][j] = 'O';
				move = 1;
			}
		}
	}
	draw(b, board);
	status = state(b);
	return status;
}

int turn(char b[3][3], int status, int who, WINDOW *board)
{
	if (who == 1) {
		mvprintw(15, 0, "Tic Tac Toe is Fun!!\n");
		status = your_turn(b, status, board);  
	} else {
		mvprintw(15, 0, "It's the Computer turn..\n");
		status = computer_turn(b, status, board);  
	}
	
	return status;
}


int state(char b[3][3])
{
	int status = 0;
	if (b[0][0] == b[0][1] && b[0][1] == b[0][2]) {
		if (b[0][0] == 'X')
			status = 1;
		else if (b[0][0] == 'O')
			status = 2;
	} else if (b[1][0] == b[1][1] && b[1][1] == b[1][2]) {
		if (b[1][0] == 'X')
			status = 1;
		else if (b[1][0] == 'O')
			status = 2;
	} else if (b[2][0] == b[2][1] && b[2][1] == b[2][2]) {
		if (b[2][0] == 'X')
			status = 1;
		else if (b[2][0] == 'O')
			status = 2;
	} else if (b[0][0] == b[1][0] && b[1][0] == b[2][0]) {
		if (b[0][0] == 'X')
			status = 1;
		else if (b[0][0] == 'O')
			status = 2;
	} else if (b[0][1] == b[1][1] && b[1][1] == b[2][1]) {
		if (b[0][1] == 'X')
			status = 1;
		else if (b[0][1] == 'O')
			status = 2;
	} else if (b[0][2] == b[1][2] && b[1][2] == b[2][2]) {
		if (b[0][2] == 'X')
			status = 1;
		else if (b[0][2] == 'O')
			status = 2;
	} else if (b[0][0] == b[1][1] && b[1][1] == b[2][2]) {
		if (b[0][0] == 'X')
			status = 1;
		else if (b[0][0] == 'O')
			status = 2;
	} else if (b[2][0] == b[1][1] && b[1][1] == b[0][2]) {
		if (b[2][0] == 'X')
			status = 1;
		else if (b[2][0] == 'O')
			status = 2;
	}
	return status;
}


WINDOW *draw_win(int h, int w, int y, int x)
{
      WINDOW *win=newwin(h,w,y,x);
	wrefresh(win);
	return win;
	}


void print(int state)
{
	if (state == 1)
		printw("You Won!!\n");
	else if (state == 2)
		printw("The Computer Won :( \n");
	else if (state == 3)
		printw("It's a draw\n");
}
