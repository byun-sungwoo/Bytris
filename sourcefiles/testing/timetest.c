#include <sys/time.h>
#include <stdio.h>

long getms(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (int)tv.tv_sec*1e6+(int)tv.tv_usec*1e-0;
}

void printtime(double sec) {
	int min = 0;
	if(sec >= 60) {
		min = (int)(sec/60);
		while(sec >= 60)
			sec -= 60;
	}
	if(min < 10)
		printf("0");
	printf("%d:",min);
	if(sec < 10)
		printf("0");
	printf("%.3f\n",sec);
}

int main(void) {
	//while(getchar() != 'q') {
	//	printf("stuff:%ld\n",getms());
	//}
	double i;
	for(i=0;i<8;i++) {
		printtime(15*i);
	}
}
