// Bytris - byun-sungwoo

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include "board.c"
#include <sys/time.h>

// game settings
#define DEBUG		0	// 1 for debug mode
#define BOXLINES	1	// 1 for boxlines
#define AUTODROP	20	// autodrop cap (seconds)
#define TICKRATE	0.8	// downtick rate (seconds)
#define LOCKDELAY	5	// lock delay (ticks)

// keyboard settings
// spacebar-32
// arrow keys {up-65, down-66, right-67, left-68}
#define SOFTDROP	66
#define HARDDROP	32
#define SONICDROP	'v'

#define ROTATELEFT	'z'
#define ROTATERIGHT	65
#define ROTATE180	'x'

#define MOVELEFT	68
#define MOVERIGHT	67

#define HOLD		'c'
#define RESET		'a'

int dimr,dimc;
void wdrawblock(WINDOW*,int,int);
void wcountdown(WINDOW*,int);
void wdrawhold(WINDOW*,int,int);
void sprint(int);
double seconds(clock_t);
long getmicro();

// main
int main(void) {
	// init ncurses and create stdscr
	initscr();
	noecho();
	// capture dimensions
	getmaxyx(stdscr, dimr, dimc);
	// no cursor
	curs_set(0);
	// color init
	start_color();
	init_pair(3,COLOR_BLACK,COLOR_GREEN);
	init_pair(13,COLOR_GREEN,COLOR_BLACK);
	init_pair(4,COLOR_BLACK,COLOR_RED);
	init_pair(14,COLOR_RED,COLOR_BLACK);
	init_pair(5,COLOR_BLACK,COLOR_WHITE);
	init_pair(15,COLOR_WHITE,COLOR_BLACK);
	init_pair(6,COLOR_BLACK,COLOR_BLUE);
	init_pair(16,COLOR_BLUE,COLOR_BLACK);
	init_pair(7,COLOR_BLACK,COLOR_MAGENTA);
	init_pair(17,COLOR_MAGENTA,COLOR_BLACK);
	init_pair(8,COLOR_BLACK,COLOR_CYAN);
	init_pair(18,COLOR_CYAN,COLOR_BLACK);
	init_pair(9,COLOR_BLACK,COLOR_YELLOW);
	init_pair(19,COLOR_YELLOW,COLOR_BLACK);
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
	int lockdelaycounter;
	int tickcounter;
	int oldboard[HEIGHT][WIDTH];
	int canhold;
	char input;
	clock_t timer,resetoffset,liveoffset,inputoffset;
	// start up game
	NEWGAME:
	holdstatus = 0;
	liveoffset = 0;
	inputoffset = 0;
	lockdelaycounter = 0;
	tickcounter = 0;
	canhold = 1;
	clear();
	wclear(well);
	wclear(stats);
	wclear(whold);
	wclear(wnext);
	wclear(message);
	if(BOXLINES) {
		box(well,0,0);
		box(stats,0,0);
		box(whold,0,0);
		box(wnext,0,0);
		box(message,0,0);
	}
	refresh();
	seed = time(NULL);
	initgame(seed);
	if(DEBUG)
		mvprintw(0,0,"seed:%d",seed);
	// draw game screen
	for(i=0;i<HEIGHT;i++)
		for(j=0;j<WIDTH;j++)
			wdrawblock(well,i,j);
	wrefresh(well);
	wrefresh(stats);
	wrefresh(whold);
	wrefresh(wnext);
	wrefresh(message);
	memcpy(oldboard,board,sizeof(int)*HEIGHT*WIDTH);
	wcountdown(message,3);
	resetoffset = getmicro();
	// game loop
	while(!gameover) {
		// update timers
		timer = getmicro()-resetoffset;
		if(seconds(timer-liveoffset) > AUTODROP) {
			// autodrop case
			harddroplive();
			canhold = 1;
			tickcounter = 0;
			liveoffset = getmicro()-resetoffset;
			inputoffset = getmicro()-resetoffset;
		} else {
			// user input
			input = getch();
			if(input == SOFTDROP)
				if(shiftlive(1,0) == 1)
					inputoffset = getmicro()-resetoffset;
			if(	input == HARDDROP	|| input == HOLD	||
				input == MOVELEFT	|| input == MOVERIGHT	||
				input == ROTATELEFT	|| input == ROTATERIGHT	||
				input == ROTATE180	|| input == SONICDROP) {
				inputoffset = getmicro()-resetoffset;
				if(input == HARDDROP) {
					harddroplive();
					canhold = 1;
					tickcounter = 0;
					liveoffset = getmicro()-resetoffset;
				}
				if(input == HOLD && canhold) {
					holdlive();
					canhold = 0;
					tickcounter = 0;
					liveoffset = getmicro()-resetoffset;
				}
				if(input == MOVELEFT)		shiftlive(0,-1);
				if(input == MOVERIGHT)		shiftlive(0,1);
				if(input == ROTATELEFT)		rotatelive(0);
				if(input == ROTATERIGHT)	rotatelive(1);
				if(input == ROTATE180)		rotate180();
				if(input == SONICDROP)		sonicdroplive();
			}
			if(input == RESET)
				goto NEWGAME;
			// downtick
			if(DEBUG)
				mvprintw(5,0,"%f > %f", seconds(timer-liveoffset), TICKRATE*(tickcounter+1));
			if(seconds(timer-liveoffset) > TICKRATE*(tickcounter+1)) {
				tickcounter++;
				int atfloor = !shiftlive(1,0);
				if(DEBUG) {
					mvprintw(6,0,"floor? %d", atfloor);
					mvprintw(7,0,"%f > %f",seconds(timer-inputoffset),TICKRATE);
				}
				lockdelaycounter += atfloor ? 1 : -lockdelaycounter;
				if(	(atfloor && seconds(timer-inputoffset)>TICKRATE) ||
					(atfloor && lockdelaycounter >= LOCKDELAY)) {
					harddroplive();
					canhold = 1;
					tickcounter = 0;
					liveoffset = getmicro()-resetoffset;
				}
			}
		}
		// update well visual
		for(i=0;i<HEIGHT;i++)
			for(j=0;j<WIDTH;j++)
				wdrawblock(well,i,j);
		// update hold visual
		if(holdstatus == 1)
			for(i=0;i<MAX-1;i++)
				for(j=0;j<MAX;j++)
					if(hold.data[i][j] == 1 && i < hold.size.row && j < hold.size.col) {
						wattron(whold,COLOR_PAIR(hold.color));
						mvwprintw(whold,i+2,3+j*2,"  ");
						wattroff(whold,COLOR_PAIR(hold.color));
					} else
						mvwprintw(whold,i+2,3+j*2,"  ");
		// update queue visual
		for(k=0;k<5;k++) {
			tetromino t = next[k];
			for(i=0;i<MAX-1;i++)
				for(j=0;j<MAX;j++)
					if(t.data[i][j] == 1 && i < t.size.row && j < t.size.col) {
						wattron(wnext,COLOR_PAIR(t.color));
						mvwprintw(wnext,i+2+(3*k),3+j*2,"  ");
						wattroff(wnext,COLOR_PAIR(t.color));
					} else
						mvwprintw(wnext,i+2+(3*k),3+j*2,"  ");
		}
		// update stat visual
		mvwprintw(stats,1,2,"%.3fs",seconds(timer));
		mvwprintw(stats,2,2,"%d/%d",linescleared,goal);
		// debug visual
		if(DEBUG) {
			// update data well visual
			for(i=1;i<HEIGHT;i++)
				for(j=0;j<WIDTH;j++)
					mvprintw(i+8,j,"%d",board[i][j]);
			mvprintw(1,0,"holdstatus:%d",holdstatus);
			mvprintw(2,0,"holdcolor:%d",hold.color);
			mvprintw(3,0,"livecolor:%d",live.color);
		}
		// reload windows
		wrefresh(well);
		wrefresh(stats);
		wrefresh(whold);
		wrefresh(wnext);
		// additional updates
		memcpy(oldboard,board,sizeof(int)*HEIGHT*WIDTH);
		// check goal reached
		if(linescleared >= goal)
			break;
	}
	if(!gameover)
		mvwprintw(message,1,2,"nice");
	else
		mvwprintw(message,1,2,"b r u h");
	wrefresh(message);
	nodelay(stdscr,FALSE);
	getch();
}

// draw the element at board[row][col] with
// respect to the given window
void wdrawblock(WINDOW *win, int row, int col) {
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
						if(DEBUG)
							mvwprintw(win,scry,scrx,"[]");
						else
							mvwprintw(win,scry,scrx,"  ");
					else
						mvwprintw(win,scry,scrx,"  ");
					wattroff(win,COLOR_PAIR(current));
				} else {
					wattron(win,COLOR_PAIR(current+10));
					mvwprintw(win,scry,scrx,"//");
					wattroff(win,COLOR_PAIR(current+10));
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

// display a countdown in the given window
void wcountdown(WINDOW *win, int countdown) {
	while(countdown >= 0) {
		if(countdown == 0)
			mvwprintw(win,1,1,"start");
		else
			mvwprintw(win,1,1,"%d",countdown);
		wrefresh(win);
		countdown--;
		sleep(1);
	}
	mvwprintw(win,1,1,"     ");
	wrefresh(win);
	flushinp();	// disregard all input that was fed during this function
}

// write the given amount of seconds
// in hr/min/s form in the window
void wtime(WINDOW *win, int y, int x, double sec) {
	// TODO
}

// given a clock_t, convert it to a double
// which is the clock_t in seconds
// basically micro sec to sec
double seconds(clock_t clock) {
	return ((double)(clock)/1000000);
}

// return the current time in micro sec
long getmicro(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (int)tv.tv_sec*1e6+(int)tv.tv_usec*1e-0;
}
