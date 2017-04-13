#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <string.h>
#include <sys/types.h>
#include <unistd.h>


#include "device4MB.h"

int fd;

void test()
{
	int k,i,sum;
	k = ioctl(fd, DEVICE4MB_IOCHELLO);
	printf("result k=%d\n",k);

	char user_msg[512]={0};
	sprintf(user_msg, "1234567890");
	k = ioctl(fd, DEVICE4MB_IOCSET, user_msg);
	printf("result k=%d\n",k);
	printf("user_msd is (%s)\n",user_msg);

	memset(user_msg,0,512);
	printf("user_msd is cleared(%s)\n",user_msg);
	k = ioctl(fd, DEVICE4MB_IOCGET, user_msg);
	printf("result k=%d\n",k);
	printf("user_msd is (%s)\n",user_msg);

	sprintf(user_msg, "0987654321");
	printf("user_msd is (%s)\n",user_msg);
	k = ioctl(fd, DEVICE4MB_IOCGETSET, user_msg);
	printf("user_msd is (%s)\n",user_msg);

	memset(user_msg,0,512);
	printf("user_msd is cleared(%s)\n",user_msg);
	k = ioctl(fd, DEVICE4MB_IOCGET, user_msg);
	printf("result k=%d\n",k);
	printf("user_msd is (%s)\n",user_msg);
}

int main(int argc, char**argv)
{
	fd = open("/dev/device4MB", O_RDWR);
	if(fd == -1)
	{
		perror("unable to open device4MB");
		exit(EXIT_FAILURE);
	}

	test();

	close(fd);
	return 0;
}
