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

	k = lseek(fd, 0, SEEK_END);
	printf("lseek = %d\n",k);

	k = lseek(fd, -4, SEEK_END);
	printf("lseek = %d\n",k);

	k = lseek(fd, -4, -1);
	printf("lseek = %d\n",k);

	//k = lseek(fd,0,SEEK_SET);
	//printf("lseek = %d\n",k);
	char buf[5*1024*1024] = {0};
	k = lseek(fd,0,SEEK_SET);
	k = read(fd, buf, 5*1024*1024);
	printf("At last, device4MB has content as %s\n",buf);
}

void initial(char i){
	char s[10];
	memset(s,i,sizeof(s));
	write(fd,s,sizeof(s));
	char c[20]="";
	int k = lseek(fd,0,SEEK_SET);
	printf("lseek = %d\n",k);
}

void testFile()
{
	int k;
	FILE * fptr;
	fptr = fopen("sampleFile", "w");
	//printf("file fptr = %d\n",fptr);
	fputs("0123456789", fptr);
	fputs("0123456789", fptr);
	//printf("file fptr = %d\n",fptr);
	//k = fseek(fptr, 0, SEEK_CUR);
	//printf("file lseek = %d\n",k);
	//fputs("ABCD", fptr);
	//k = fseek(fptr, 2, SEEK_SET);
	//k = fseek(fptr, 2, SEEK_SET);
	//printf("file lseek = %d\n",k);
	//fputs("abc", fptr);
	//k = fseek(fptr, 0, SEEK_CUR);
	//printf("file lseek = %d\n",k);
	fclose(fptr);
}

int main(int argc, char ** argv){
	fd = open("/dev/device4MB", O_RDWR);
	if(fd==-1){
		printf("unable to open device4MB");
		exit(EXIT_FAILURE);
	}
	initial('1');
	test();
	//testFile();
	close(fd);
	return 0;
}

