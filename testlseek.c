#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


int fd;

void test(){
	int k,i,sum;
	char s[3];
	memset(s, '2', sizeof(s));

	printf("test begin!\n");

	k = lseek(fd, 4, SEEK_CUR);
	printf("lseek = %d\n",k);

	//char buf[5*1024*1024] = {0};
	//k = read(fd, buf,  6*1024*1024);
	//printf("read k = %d\n",k);


	k = write(fd, s, sizeof(s));
	printf("write = %d\n",k);

	//k = lseek(fd, 0, SEEK_END);
	//printf("lseek = %d\n",k);

	//k = lseek(fd, -4, SEEK_END);
	//printf("lseek = %d\n",k);

	//k = lseek(fd, -4, -1);
	//printf("lseek = %d\n",k);
}

void initial(char i){
	char s[10];
	memset(s,i,sizeof(s));
	write(fd,s,sizeof(s));
	char c[20]="";
	int k = lseek(fd,0,SEEK_SET);
	printf("lseek = %d\n",k);
}

int main(int argc, char ** argv){
	fd = open("/dev/device4MB", O_RDWR);
	if(fd==-1){
		printf("unable to open device4MB");
		exit(EXIT_FAILURE);
	}
	initial('1');
	test();
	close(fd);
	return 0;
}

