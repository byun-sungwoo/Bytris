// Bytris - byun-sungwoo

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../tetromino.c"

// TEST FILE
int main(void) {
	int right = 1;	// Set this to 0 for left rotation
	tetromino tet;
	int i,j;
	if(right) printf("[ROTATING RIGHT]\n");
	else printf("[ROTATING LEFT]\n");
	
	for(i=0;i<7;i++) {
		printf("----------BLOCK %d TEST----------\n", i);
		tet = pool[i];
		printinfo(&tet);
		printdata(&tet);
		printf("\n");
		for(j=0;j<4;j++) {
			rotate(&tet,right);
			printinfo(&tet);
			printdata(&tet);
			printf("\n");
		}
	}
	return 0;
}
