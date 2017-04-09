#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char** argv)
{
	printf("\nTest begin");
	printf("\nNow creating a 5MB sample file");
	FILE * fptr;
	fptr = fopen("sampleFile", "w");
	if(fptr==NULL)
	{
		printf("\nFile create fails\n");
		return 0;
	}
	int i = 0, j=0, k=0;
	char data[9] = {0};
	int temp1, temp2;
	for(i=0; i<0x14000; i++)
	//for(i=0; i<10; i++)
	{
		for(k=0;k<8;k++)
		{
			for(j=0;j<5;j++)
			{
				//data[j] = (char)(i%(int)(pow(10,(5-j))))/(pow(10,(4-j))+ '0');
				temp1 = (int)pow((double)10,(double)(5-j));
				temp2 = (int)pow((double)10,(double)(4-j));
				data[j] = (char)(((i%temp1)/temp2)+'0');

			}
			data[j++] = ' ';
			data[j++] = (char) (k+'0');
			data[j++] = '\n';
			data[j++] = 0;

			fprintf(fptr, "%s", data);
		}
		
	}
	fclose(fptr);

	int ret, fd;
	fd = open("/dev/device4MB", O_RDWR);
	if(fd < 0)
	{
		perror("Failed to open the device...");
		return errno;
	}
	char buf[6*1024*1024];
	long numR,numW;
	fptr = fopen("sampleFile", "r");
	if(fptr==NULL)
	{
		printf("\nFile create fails\n");
		return errno;
	}
	numR = fread(buf, 1, 6*1024*1024, fptr);
	printf("\nRead %ld byte from File",numR);
	printf("\nNow write into device4MB device...");
	numW = write(fd, buf, numR);
	if(numW < 0)
	{
		perror("Failed to write the message to the device.");
		return errno;
	}
	printf("\nSuccessfully write %ld byte to device4MB", numW);
	printf("\nNow read from device4MB...");
	memset(buf,0, 6*1024*1024);
	numR = read(fd, buf,  6*1024*1024);
	printf("\nSuccessfully read %ld byte from device4MB",numR);
	printf("\nFirst 8 bytes from device4MB is: ");
	for(i=0; i<8; i++)
	{
		printf("%c ", buf[i]);
	}
	printf("\nLast 8 bytes from device4MB is: ");
	for(i=0; i<8; i++)
	{
		printf("%c ", buf[numR-8+i]);
	}
	close(fd);
	printf("\nTest done\n");
	return 1;
}
