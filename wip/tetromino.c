// Bytris - byun-sungwoo

#include "tetromino.h"

// update rowcol struct values
void setrowcol(rowcol *rc, int row, int col) {
	rc->row = row;
	rc->col = col;
}

// 1 clockwise, 0 counterclockwise
void rotate(tetromino *block, int clockwise) {
	int i,j,n,m;
	int row = block->size.row;
	int col = block->size.col;
	int rotate[MAX][MAX];
	int update = 1;
	for(i = 0; i < MAX; i++)
		for(j = 0; j < MAX; j++)
			if(i < row && j < col)
				if(clockwise)
					rotate[j][row-1-i] = block->data[i][j];
				else
					rotate[col-1-j][i] = block->data[i][j];
			else
				rotate[j][i] = 0;
	int n_old = block->pivot.row;
	if(clockwise) {
		block->pivot.row = block->pivot.col;
		block->pivot.col = row-1-n_old-block->mode;
	} else {
		block->pivot.row = col-1-block->pivot.col-block->mode;
		block->pivot.col = n_old;
	}
	block->size.row = col;
	block->size.col = row;
	if(clockwise)
		block->state++;
	else
		block->state--;
	if(block->state < 0)
		block->state = 3;
	if(block->state > 3)
		block->state = 0;
	memcpy(block->data,rotate,sizeof(int)*MAX*MAX);
}

// print tetromino data
void printdata(tetromino *block) {
	int i,j;
	for(i = 0; i < block->size.row; i++) {
		printf("[");
		for(j = 0; j < block->size.col; j++) {
			printf("%d", block->data[i][j]);
			if(j+1 < block->size.col)
				printf(",");
		}
		printf("]\n");
	}
}

// print all tetromino information excluding data
void printinfo(tetromino *block) {
	printf(	"mode:%d\tstate:%d\tsize:%d,%d\tpivot:%d,%d\tpos:%d,%d\tcolor:%d\n",
		block->mode,block->state,
		block->size.row,block->size.col,
		block->pivot.row,block->pivot.col,
		block->pos.row,block->pos.col,
		block->color);
}
