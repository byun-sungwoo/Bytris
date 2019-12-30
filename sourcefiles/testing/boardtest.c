// Bytris - byun-sungwoo

#include <stdio.h>
#include <time.h>
#include "../board.c"

int main(void) {
	int i,j,k;
	
	initgame(time(NULL));
	for(j=0;j<7;j++) {
		printf("LIVE\n");
		printdata(&live);
		printf("NEXT\n");
		for(i=0;i<7;i++) {
			printdata(&next[i]);
			printf("\n");
		}
		popnext();
	}
	displaygame();
	
	popnext();
	printf("LIVE\n");
	printdata(&live);
	printf("NEXT\n");
	for(i=0;i<7;i++) {
		printdata(&next[i]);
		printf("\n");
	}
	displaygame();

	shiftlive(0,0);
	displaygame();
	printf("\n");
	printdata(&ghost);
	printinfo(&ghost);

	shiftlive(1,0);
	displaygame();
	printf("\n");
	printdata(&ghost);
	printinfo(&ghost);

	rotatelive(0);
	displaygame();
	printf("\n");
	printdata(&ghost);
	printinfo(&ghost);

	shiftlive(0,4);
	displaygame();
	printf("\n");
	printdata(&ghost);
	printinfo(&ghost);

	rotatelive(1);
	displaygame();
	printf("\n");
	printdata(&ghost);
	printinfo(&ghost);

	harddroplive();
	displaygame();
	printf("\n");
	printdata(&ghost);
	printinfo(&ghost);

	shiftlive(1,0);
	displaygame();
	printf("\n");
	printdata(&ghost);
	printinfo(&ghost);

	harddroplive();
	displaygame();
	printf("\n");
	printdata(&ghost);
	printinfo(&ghost);
//	for(i=0;i<WIDTH;i++)
//		board[20][i] = 2;
//	for(i=0;i<WIDTH;i++)
//		board[19][i] = 2;
//	for(i=0;i<WIDTH;i++)
//		if(i!=3)board[18][i] = 2;
//	for(i=0;i<WIDTH;i++)
//		board[17][i] = 2;
//	for(i=0;i<WIDTH;i++)
//		if(i!=2&&i!=6)board[16][i] = 2;
//	for(i=0;i<WIDTH;i++)
//		board[15][i] = 2;
//	for(i=0;i<WIDTH;i++)
//		if(i!=2&&i!=1&&i!=6)board[14][i] = 2;
//	displaygame();
//	clearlines(20,14);
//	displaygame();
	return 0;
}
