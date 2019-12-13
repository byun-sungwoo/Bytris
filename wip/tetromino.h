// 2019-12-07
// Bytris - byun-sungwoo

#define MAX 4

typedef struct rowcol {
	int row;
	int col;
} rowcol;

const rowcol spawn = {1,4};

typedef struct tetromino {
	int mode;		// rotation mode: 0 normal, 1 offset
	int state;		// block state	: 0 spawn, 1 right, 2 180, 3 left
	rowcol size;		// size, 1 index
	rowcol pivot;		// pivot point, 0 index
	rowcol pos;		// position on board, 0 index
	int color;		// block color, 0 reserved empty, 1 reserved live, 2 reserved ghost
	int data[MAX][MAX];	// tetris block data
} tetromino;

const tetromino pool[7] = {	{0,0,{2,3},{1,1},spawn,3,{{0,1,1},{1,1,0}}},	// s
				{0,0,{2,3},{1,1},spawn,4,{{1,1,0},{0,1,1}}},	// z
				{0,0,{2,3},{1,1},spawn,5,{{0,0,1},{1,1,1}}},	// l
				{0,0,{2,3},{1,1},spawn,6,{{1,0,0},{1,1,1}}},	// j
				{0,0,{2,3},{1,1},spawn,7,{{0,1,0},{1,1,1}}},	// t
				{1,0,{1,4},{0,1},spawn,8,{{1,1,1,1}}},		// i
				{1,0,{2,2},{0,0},{0,4},9,{{1,1},{1,1}}}};	// o

void setrowcol(rowcol*,int,int);
void rotate(tetromino*,int);
void printdata(tetromino*);
void printinfo(tetromino*);
