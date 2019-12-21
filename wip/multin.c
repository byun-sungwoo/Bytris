#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include "board.c"

int main(void) {
	// multi input start
	char input;
	char *inputs;
	int size = 0;
	printf("%s\n\n",inputs);
	
	input = 'a';
	strcat(inputs,&input);
	printf("%s\n\n",inputs);

	input = 'b';
	strcat(inputs,&input);
	printf("%s\n\n",inputs);
	
	input = 'c';
	strcat(inputs,&input);
	printf("%s\n\n",inputs);

	// trying to shift all inputs from 1-n to 0-(n-1)
	printf("pop %c\n",inputs[0]);
	memcpy(inputs[0],inputs[1],sizeof(int));
	printf("%s\n\n",inputs);
	return 0;
}
