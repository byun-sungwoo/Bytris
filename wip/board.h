// Bytris - byun-sungwoo

#include "tetromino.c"

#define WIDTH 10
#define HEIGHT 21
#define BAGSIZE 7

void initgame(int);
void updateghost();
void initnext();
void popnext();
void holdlive();
void displaygame();
int rotate180();
int rotatelive(int);
int rotateleftlive();
int rotaterightlive();
int shiftlive(int,int);
int validblock(tetromino*,int,int);
int clearlines(int,int);
int harddropshiftlive();
int harddroplive();
int locklive();
int placeblock(tetromino*);
int removeblock(tetromino*);
