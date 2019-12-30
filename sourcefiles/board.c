// Bytris - byun-sungwoo

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "board.h"

tetromino live;			// curent block
tetromino ghost;		// ghost block
tetromino next[BAGSIZE];	// next BAGSIZE blocks, display only 5
tetromino hold;			// hold block
int gameover = 0;		// if 1, game over
int board[HEIGHT][WIDTH];	// represent the game board
int holdstatus = 0;
int bag[BAGSIZE];
int bagstatus = 0;
int linescleared = 0;

// init game.
void initgame(int seed) {
	linescleared = 0;
	memset(board,0,sizeof(int)*HEIGHT*WIDTH);
	//printf("seed:%d\n",seed);
	srand(seed);
	int i;
	for(i=0;i<BAGSIZE;i++)
		bag[i] = 0;
	initnext();
	popnext();
}

// update values and position of ghost piece.
void updateghost() {
	removeblock(&ghost);
	ghost = live;
	int i,j;
	ghost.pos.row = live.pos.row+harddropshiftlive();
	placeblock(&ghost);
}

// populate next with BAGSIZE unique tetrominos from pool.
void initnext() {
	int option[BAGSIZE];	// values dont matter as long as they aren't -1
	int i;
	for(i=0;i<BAGSIZE;i++)
		option[i] = 0;
	int total = 0;
	int temp;
	while(total < BAGSIZE) {
		temp = (int)((((double)rand())/((double)RAND_MAX))*BAGSIZE);
		if(option[temp] != -1) {
			next[total] = pool[temp];
			total++;
			option[temp] = -1;
		}
	}
}

// set next[0] as live, shift all next elements over
// by 1 and generate new next value.
void popnext() {
	int temp;
	removeblock(&live);
	live = next[0];
	memcpy(&next[0],&next[1],sizeof(tetromino)*BAGSIZE);
	while(1) {
		temp = (int)((((double)rand())/((double)RAND_MAX))*BAGSIZE);
		if(bag[temp] != -1) {
			next[6] = pool[temp];
			bagstatus++;
			bag[temp] = -1;
			break;
		}
	}
	if(bagstatus >= BAGSIZE) {
		int tmpbag[BAGSIZE];
		int i;
		for(i=0;i<BAGSIZE;i++)
			tmpbag[i] = 0;
		memcpy(&bag[0],&tmpbag[0],sizeof(int)*BAGSIZE);
		bagstatus = 0;
	}
	gameover = !shiftlive(0,0);
}

// holds the live block. if hold block is empty, just set the hold
// equal to live and generate a new live block. otherwise swap
// the hold block and live block.
void holdlive() {
	int ofs = 3;
	if(holdstatus == 0) {
		holdstatus = 1;
		memcpy(&hold,&pool[live.color-ofs],sizeof(tetromino));
		popnext();
	} else {
		// says to just set them equal to eachother
		removeblock(&live);
		int ohc = hold.color;
		memcpy(&hold,&pool[live.color-ofs],sizeof(tetromino));
		memcpy(&live,&pool[ohc-ofs],sizeof(tetromino));
		shiftlive(0,0);
	}
}

// temporary display of game.
// print out board.
void displaygame() {
	int i,j,k;
	for(i=0;i<HEIGHT;i++) {
		if(i == 0) {
			printf("  ");
			for(k=0;k<WIDTH;k++)
				printf(" %d",k);
			printf("\n");
		} else {
		printf("%2d[",i);
			for(j=0;j<WIDTH;j++) {
				printf("%d",board[i][j]);
				if(j+1!=WIDTH)
					printf(" ");
				else
					printf("]\n");
			}
		}
	}
	printf(	"pos:%d,%d\n",
		live.pos.row,
		live.pos.col);
}

int rotate180() {
	removeblock(&live);
	rotate(&live,0);
	rotate(&live,0);
	int state = live.state;
	int i;
	if(validblock(&live,live.pos.row,live.pos.col)) {	// test 1
		shiftlive(0,0);
		return 1;
	}
	if(live.mode == 0) {			// standard
		int lstr[4] = {0,1,-2,-2};
		int lstc[4] = {1,1,0,1};
		int sc = state == 0 || state == 3 ? 1 : -1;
		int sr = state == 1 || state == 3 ? 1 : -1;
		for(i=0;i<4;i++) {		// tests 2-5
			int tmpr = -lstr[i]*sr;
			int tmpc = lstc[i]*sc;
			if(validblock(&live,live.pos.row+tmpr,live.pos.col+tmpc)) {
				shiftlive(tmpr,tmpc);
				return 1;
			}
		}
	} else {				// offset
		int lstr[4] = {0,0,2,-1};
		int lstc[4] = {-1,2,-1,2};
		int swp = state == 0 || state == 2 ? 1 : 0;
		int nr = state == 0 || state == 1 ? 1 : 0;
		int sc = state == 0 || state == 2 ? 1 : 0;
		int nc = state == 0 || state == 1 ? 1 : 0;
		for(i=0;i<4;i++) {		// tests 2-5
			int tmpr = -lstr[i];
			int tmpc = lstc[i];
			if(swp && tmpr != 0) tmpr = tmpc;
			if(nr) tmpr *= -1;
			if(sc) tmpc--;
			if(nc) tmpc *= -1;
			if(validblock(&live,live.pos.row+tmpr,live.pos.col+tmpc)) {
				shiftlive(tmpr,tmpc);
				return 1;
			}
		}
	}
	rotate(&live,0);
	rotate(&live,0);
	placeblock(&live);
	return 0;
}

// if right = 1, rotate right
int rotatelive(int right) {
	int n = right ? rotaterightlive() : rotateleftlive();
	return n;
}

// Rotate live block to the left, if not possible,
// try 5 tests. If they all fail, do nothing.
// return 1 if success, 0 if no change.
int rotateleftlive() {
	removeblock(&live);
	rotate(&live,0);
	int state = live.state;
	int i;
	if(validblock(&live,live.pos.row,live.pos.col)) {	// test 1
		shiftlive(0,0);
		return 1;
	}
	if(live.mode == 0) {			// standard
		int lstr[4] = {0,1,-2,-2};
		int lstc[4] = {1,1,0,1};
		int sc = state == 0 || state == 3 ? 1 : -1;
		int sr = state == 1 || state == 3 ? 1 : -1;
		for(i=0;i<4;i++) {		// tests 2-5
			int tmpr = -lstr[i]*sr;
			int tmpc = lstc[i]*sc;
			if(validblock(&live,live.pos.row+tmpr,live.pos.col+tmpc)) {
				shiftlive(tmpr,tmpc);
				return 1;
			}
		}
	} else {				// offset
		int lstr[4] = {0,0,2,-1};
		int lstc[4] = {-1,2,-1,2};
		int swp = state == 0 || state == 2 ? 1 : 0;
		int nr = state == 0 || state == 1 ? 1 : 0;
		int sc = state == 0 || state == 2 ? 1 : 0;
		int nc = state == 0 || state == 1 ? 1 : 0;
		for(i=0;i<4;i++) {		// tests 2-5
			int tmpr = -lstr[i];
			int tmpc = lstc[i];
			if(swp && tmpr != 0) tmpr = tmpc;
			if(nr) tmpr *= -1;
			if(sc) tmpc--;
			if(nc) tmpc *= -1;
			if(validblock(&live,live.pos.row+tmpr,live.pos.col+tmpc)) {
				shiftlive(tmpr,tmpc);
				return 1;
			}
		}
	}
	rotate(&live,1);	// if here, all failed. do nothing.
	placeblock(&live);
	return 0;
}

// Rotate live block to the right, if not possible,
// try 5 tests. If they all fail, do nothing.
// return 1 if success, 0 if no change.
int rotaterightlive() {
	removeblock(&live);
	rotate(&live,1);
	int state = live.state;
	int i;
	if(validblock(&live,live.pos.row,live.pos.col)) {	// test 1
		shiftlive(0,0);
		return 1;
	}
	if(live.mode == 0) {			// standard
		int lstr[4] = {0,-1,2,2};
		int lstc[4] = {-1,-1,0,-1};
		int sc = state == 2 || state == 3 ? -1 : 1;
		int sr = state == 1 || state == 3 ? -1 : 1;
		for(i=0;i<4;i++) {		// tests 2-5
			int tmpr = -lstr[i]*sr;
			int tmpc = lstc[i]*sc;
			if(validblock(&live,live.pos.row+tmpr,live.pos.col+tmpc)) {
				shiftlive(tmpr,tmpc);
				return 1;
			}
		}
	} else {				// offset
		int lstr[4] = {0,0,-2,1};
		int lstc[4] = {1,-2,1,-2};
		int swp = state == 1 || state == 3 ? 1 : 0;
		int nr = state == 1 || state == 2 ? 1 : 0;
		int ac = state == 1 || state == 3 ? 1 : 0;
		int nc = state == 1 || state == 2 ? 1 : 0;
		for(i=0;i<4;i++) {		// tests 2-5
			int tmpr = -lstr[i];
			int tmpc = lstc[i];
			if(swp && tmpr != 0) tmpr = tmpc;
			if(nr) tmpr *= -1;
			if(ac) tmpc++;
			if(nc) tmpc *= -1;
			if(validblock(&live,live.pos.row+tmpr,live.pos.col+tmpc)) {
				shiftlive(tmpr,tmpc);
				return 1;
			}
		}
	}
	rotate(&live,0);	// if here, all failed. do nothing.
	placeblock(&live);
	return 0;
}

// move live piece row and col amount on the board.
// if move is not valid return 0, else move and return 1.
int shiftlive(int row, int col) {
	if(validblock(&live,live.pos.row+row,live.pos.col+col)) {
		removeblock(&live);
		live.pos.row+=row;
		live.pos.col+=col;
		updateghost();
		placeblock(&live);
		return 1;
	}
	return 0;
}

// checks if placing the block at its pivot at pos row,col
// is valid when row and col are zero index.
// Returns 1 if it is valid, 0 otherwise.
int validblock(tetromino *block, int row, int col) {
	int n = block->pivot.row;
	int m = block->pivot.col;
	int r = block->size.row;
	int c = block->size.col;
	// need to check for placed blocks
	if(row-n < 0 || col-m < 0)
		return 0;
	if(row+r-1-n >= HEIGHT || col+c-1-m >= WIDTH)
		return 0;
	int i,j;
	for(i=0;i<r;i++)
		for(j=0;j<c;j++)
			if(	board[row-n+i][col-m+j] >= 3 &&
				block->data[i][j] == 1)
				return 0;
	return 1;
}

// Deletes full lines and shifts down lines above.
// only checks between low and high
// lb=lowerbound, up=upperbound. lb>=ub
int clearlines(int lb, int ub) {
	if(lb<ub) {
		int swp = lb;
		lb = ub;
		ub = swp;
	}
	// make sure to check all line and shift down
	int i,j,counter,cleared=0;
	for(i=lb;i>=ub;i--) {
		counter = 0;
		for(j=0;j<WIDTH;j++)
			counter += board[i+cleared][j] >= 3;
		if(counter==WIDTH) {
			int shifted;
			int cur = 1;
			do {
				shifted = 0;
				for(j=0;j<WIDTH;j++) {
					if(board[i+cleared-cur][j] >= 3) {
						board[i+cleared-cur+1][j] = board[i+cleared-cur][j];
						shifted++;
					}
					else
						board[i+cleared-cur+1][j] = 0;
				}
				cur++;
			} while(shifted != 0);
			cleared++;
		}
	}
	return cleared;
}

// returns how many rows down hdrop should shift live
int harddropshiftlive() {
	int i;
	int c=0;
	int lines = 0;
	for(i=live.pos.row;i<HEIGHT+1;i++) {
		if(!validblock(&live,i,live.pos.col))
			break;
		c++;
	}
	return c-1;
}

// hard drop, basically what spacebar does in tetris.
// returns number of lines cleared
int harddroplive() {
	int lines = 0;
	shiftlive(harddropshiftlive(),0);
	lines = locklive();
	linescleared += lines;
	return lines;
}

// somewhat mimics the sonic drop function
// by moving the live block to the lowest it can go
int sonicdroplive() {
	return shiftlive(harddropshiftlive(),0);
}

// live piece will get updated.
// calls popnext which will update pos.
// returns number of lines cleared
int locklive() {
	int i,j;
	for(i = 0; i < live.size.row; i++)
		for(j = 0; j < live.size.col; j++)
			if(live.data[i][j] == 1)
				board[live.pos.row+i-live.pivot.row][live.pos.col+j-live.pivot.col] = live.color;
	int lines = clearlines(live.pos.row-live.size.row,live.pos.row+live.size.row-1-live.pivot.row);
	popnext();	// update live.pos.row,live.pos.col and generate new live
	return lines;
}

// if the block is not out of bounds, place the block according to
// its pivot at pos row,col which are both zero index.
// update live pos.
int placeblock(tetromino *block) {
	if(validblock(block,block->pos.row,block->pos.col)) {
		int i,j;
		for(i = 0; i < block->size.row; i++)
			for(j = 0; j < block->size.col; j++)
				if(block->data[i][j] == 1)
					board[block->pos.row+i-block->pivot.row][block->pos.col+j-block->pivot.col] = block->data[i][j];
		return 1;
	}
	return 0;
}

// if the block is not out of bounds, remove the block according to
// its pivot at pos row,col which are both zero index.
int removeblock(tetromino *block) {
	if(validblock(block,block->pos.row,block->pos.col)) {
		int i,j;
		for(i = 0; i < block->size.row; i++)
			for(j = 0; j < block->size.col; j++)
				if(block->data[i][j] == 1)
					board[block->pos.row+i-block->pivot.row][block->pos.col+j-block->pivot.col] = 0;
		return 1;
	}
	return 0;
}
