#ifndef TICTACTOE_H
# define TICTACTOE_H
#include <ncurses.h>
#include<curses.h>

void draw(char b[3][3], WINDOW* board_win);

int state(char b[3][3]);

int turn( char b [3][3], int stat, int who, WINDOW *board_win);

void print(int stat);

int your_turn(char b[3][3], int stat, WINDOW *board_win);

int computer_turn(char b[3][3], int stat, WINDOW *board_win);

WINDOW *draw_win(int h, int w, int y, int x);



#endif
