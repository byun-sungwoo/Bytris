#include <sys/time.h>
#include <stdio.h>

long getms(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (int)tv.tv_sec*1e6+(int)tv.tv_usec*1e-0;
}

int main(void) {
	printf("stuff:%d\n",(int)1e2);
	printf("stuff:%f\n",1e-2);
	while(getchar() != 'q')
		printf("stuff:%ld\n",getms());
}
