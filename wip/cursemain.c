// 2019-12-07
// Bytris - byun-sungwoo

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include "board.c"
#define AUTODROP 20
#define TICKRATE 0.8
#define HARDDROP	32
#define ROTATERIGHT	65
#define SOFTDROP	66
#define MOVERIGHT	67
#define MOVELEFT	68
#define HOLD		99
#define ROTATELEFT	122
#define RESET		'r'

// Implemented:
// Gamemode		: Sprint
// Rotation System	: SRS
// Block Generation	: Unique rotation, 7 bag following Tetris 99
// Board Dimension	: 21 rows 10 col, game displays bottom 20
// Ghost Block		: Done
// Lock Delay		: Fixed time from spawn
// Next Pieces		: Done
// Reset Game		: Done
// 
// TODO:
// Hold Block		: WIP
// Spring Leaderboard	: Need to read/write txt file
// Gamemode		: Marathon & Battle
// Game menu		: Maybe
// Input		: Add DAS/ARR

int dimr,dimc;
void wdrawblock(WINDOW*,int,int,int);
void wdrawhold(WINDOW*,int,int);
void sprint(int);
double seconds(clock_t);

int main(void) {
	// init ncurses
	initscr();	// create stdscr
	noecho();
	// capture dimensions
	getmaxyx(stdscr, dimr, dimc);
	// no cursor
	curs_set(0);
	// color init
	start_color();
	init_pair(3,COLOR_BLACK,COLOR_GREEN);
	init_pair(4,COLOR_BLACK,COLOR_RED);
	init_pair(5,COLOR_BLACK,COLOR_WHITE);
	init_pair(6,COLOR_BLACK,COLOR_BLUE);
	init_pair(7,COLOR_BLACK,COLOR_MAGENTA);
	init_pair(8,COLOR_BLACK,COLOR_CYAN);
	init_pair(9,COLOR_BLACK,COLOR_YELLOW);
	// gamemode sprint
	sprint(40);
	// end ncurses
	while(1)
		if(getchar() == 'q')
			break;
	endwin();
	return 0;
}

// sprint gamemode
void sprint(int goal) {
	nodelay(stdscr,TRUE);
	// init game screen
	WINDOW *well = newwin(	HEIGHT-1+2,(WIDTH+1)*2,
				(dimr/2)-((HEIGHT-1+2)/2),
				(dimc/2)-(WIDTH+2));
	WINDOW *stats = newwin(	5,(WIDTH+1)*2,
				(dimr/2)+((HEIGHT-1+2)/2),
				(dimc/2)-(WIDTH+2));
	WINDOW *whold = newwin(	6,6*2,
				(dimr/2)-((HEIGHT-1+2)/2),
				(dimc/2)-(WIDTH+1)-7*2);
	WINDOW *wnext = newwin(	4*5-2,6*2,
				(dimr/2)-((HEIGHT-1+2)/2),
				(dimc/2)+(WIDTH+1));
	WINDOW *message = newwin(3,(WIDTH+1)*2,
				(dimr/2)-((HEIGHT-1+2)/2)-3,
				(dimc/2)-(WIDTH+2));
	// tetris variables
	int seed,i,j,k;
	char input;
	clock_t timer,resetoffset,liveoffset,inputoffset;
	resetoffset = 0;
	int lockdelaycap = 5;
	int lockdelaycounter = 0;
	int tickcounter = 0;
	int oldboard[HEIGHT][WIDTH];
	// start up tetris
	NEWGAME:
	clear();
	wclear(well);
	wclear(stats);
	wclear(whold);
	wclear(wnext);
	wclear(message);
	box(well,0,0);
	box(stats,0,0);
	box(whold,0,0);
	box(wnext,0,0);
	box(message,0,0);
	refresh();
	seed = time(NULL);
	initgame(seed);
	mvprintw(0,0,"seed:%d",seed);
	// draw game screen
	for(i=0;i<HEIGHT;i++)
		for(j=0;j<WIDTH;j++)
			wdrawblock(well,i,j,0);
	wrefresh(well);
	wrefresh(stats);
	wrefresh(whold);
	wrefresh(wnext);
	wrefresh(message);
	liveoffset = 0;
	inputoffset = 0;
	lockdelaycounter = 0;
	tickcounter = 0;
	memcpy(oldboard,board,sizeof(int)*HEIGHT*WIDTH);
	while(!gameover) {
		// update timers
		timer = clock()-resetoffset;
		if(seconds(timer-liveoffset) > AUTODROP) {	// autodrop
			harddroplive();
			tickcounter = 0;
			liveoffset = clock();
			inputoffset = clock();
		} else {			// user input
			input = getch();
			if(input == SOFTDROP) {
				int check = shiftlive(1,0);
				if(check == 1)
					inputoffset = clock();
			}
			if(input == HARDDROP) {
				harddroplive();
				tickcounter = 0;
				liveoffset = clock();
				inputoffset = clock();
			}
			if(input == MOVELEFT) {
				shiftlive(0,-1);
				inputoffset = clock();
			}
			if(input == MOVERIGHT) {
				shiftlive(0,1);
				inputoffset = clock();
			}
			if(input == ROTATELEFT) {
				rotatelive(0);
				inputoffset = clock();
			}
			if(input == ROTATERIGHT) {
				rotatelive(1);
				inputoffset = clock();
			}
			if(input == HOLD) {
				//holdlive();
				//tickcounter = 0;
				//liveoffset = clock();
				//inputoffset = clock();
			}
			if(input == RESET) {
				resetoffset = clock();
				goto NEWGAME;
			}
			// downtick
			if(seconds(timer-liveoffset) > TICKRATE*(tickcounter+1)) {
				tickcounter++;
				int atfloor = !shiftlive(1,0);
				lockdelaycounter += atfloor ? 1 : -lockdelaycounter;
				if(	(lockdelaycounter >= lockdelaycap) || 
					(atfloor && seconds(timer-inputoffset)>TICKRATE)) {
					harddroplive();
					tickcounter = 0;
					liveoffset = clock();
				}
			}
		}
		// update well visual
		int ghosttracker = 0;
		for(i=0;i<HEIGHT;i++)
			for(j=0;j<WIDTH;j++)
				if(1 || board[i][j] != oldboard[i][j] || (board[i][j] == 1 && i<2)) {
					if(board[i][j] == 1)
						ghosttracker++;
					wdrawblock(well,i,j,ghosttracker);
				}
		// data well visual
		for(i=1;i<HEIGHT;i++)
			for(j=0;j<WIDTH;j++)
				mvprintw(i+9,j,"%d",board[i][j]);
		// update stat visual
		mvwprintw(stats,1,2,"%.3fs",seconds(timer));
		mvwprintw(stats,2,2,"%d/%d",linescleared,goal);
		// update whold visual
		for(i=0;i<hold.size.row;i++)
			for(j=0;j<hold.size.col;j++) {
				if(hold.data[i][j] == 1) {
					wattron(whold,COLOR_PAIR(hold.color));
					mvwprintw(whold,i+1,1+j*2,"  ");
					wattroff(whold,COLOR_PAIR(hold.color));
				} else
					mvwprintw(whold,i+1,1+j*2,"  ");
			}
		// update next visual
		for(k=0;k<5;k++) {
			tetromino t = next[k];
			for(i=0;i<MAX-1;i++) {
				for(j=0;j<MAX;j++) {
					if(t.data[i][j] == 1 && i < t.size.row && j < t.size.col) {
						wattron(wnext,COLOR_PAIR(t.color));
						mvwprintw(wnext,i+2+(3*k),3+j*2,"  ");
						wattroff(wnext,COLOR_PAIR(t.color));
					} else
						mvwprintw(wnext,i+2+(3*k),3+j*2,"  ");
				}
			}
		}
		// reload sections
		wrefresh(well);
		wrefresh(stats);
		wrefresh(whold);
		wrefresh(wnext);
		// additional updates
		memcpy(oldboard,board,sizeof(int)*HEIGHT*WIDTH);
		// check goal reached
		if(linescleared >= goal)
			break;
		mvprintw(0,20,"holdstatus:%d",holdstatus);
		mvprintw(1,20,"holdcolor:%d",hold.color);
		mvprintw(2,20,"livecolor:%d",live.color);
	}
	if(!gameover)
		mvwprintw(message,1,2,"nice");
	else
		mvwprintw(message,1,2,"b r u h");
	wrefresh(message);
	nodelay(stdscr,FALSE);
	getch();
}

void wdrawblock(WINDOW *win, int row, int col, int ghosttracker) {
	if(row >= 1 && row < HEIGHT && col >= 0 && col < WIDTH) {
		int current = board[row][col];
		int scry = row;
		int scrx = (col*2)+1;
		if(current == 0)
			mvwprintw(win,scry,scrx,". ");
		else {
			if(current == 1)
				current = live.color;
			int r = row - live.pos.row;
			int c = col - live.pos.col;
			if(board[row][col] == 1) {
				if(live.data[live.pivot.row+r][live.pivot.col+c] == 1) {
					wattron(win,COLOR_PAIR(current));
					if(row == live.pos.row && col == live.pos.col)
						mvwprintw(win,scry,scrx,"[]");
					else
						mvwprintw(win,scry,scrx,"  ");
					wattroff(win,COLOR_PAIR(current));
				} else {
					mvwprintw(win,scry,scrx,"//");
				}
			}
			else {
				wattron(win,COLOR_PAIR(current));
				mvwprintw(win,scry,scrx,"  ",current);
				wattroff(win,COLOR_PAIR(current));
			}
		}
	}
}

double seconds(clock_t clock) {
	return (double)(clock)/1000000;
}
