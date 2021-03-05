#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>

#include "sbull.h"

#define BUFF_SIZE 4096
#define SIZE 1024*1024*512/4096

int main(){
    char buf[BUFF_SIZE];
    int loop, random = 0;
    int fd1 = 0, fd2 = 0, dev = 0;
    ssize_t rd_size;
    clock_t write_start, write_end, read_start, read_end;

    srand(time(NULL));

	int testloop = 0;
	for (; testloop < 2; testloop++) {

		/*----------------------------------------------*/

		if((dev = open("/dev/sbulla",O_RDWR)) < 0){
			printf("Device open error\n");
			exit(1);
		}
		ioctl(dev, SBULL_CLEAR_INFO);
		if (testloop == 0) {
			ioctl(dev, SBULL_DISABLE_COMPRESS);
		} else {
			ioctl(dev, SBULL_ENABLE_COMPRESS);
		}
		fsync(dev);

		/*----------------------------------------------*/

		write_start = clock();
		loop = 0;
		if(0 < (fd1 = open("sample1",O_RDONLY))){
			while( 0 < (rd_size = read(fd1, buf, BUFF_SIZE))){
				if(write(dev, buf, rd_size) < 0){
					perror("Write error");
					exit(1);
				}
				random = rand()%10000000;
				lseek(fd1, random, SEEK_SET);
				if(random >= 95536){
					lseek(fd1, 0, SEEK_SET);
				}
				if(loop % 1000 == 0){
					//printf("sample 1, write : %d/65536\n",loop);
				}
				if(loop == 65537){
					break;
				}
				loop += 1;
			}
		}else{
			printf("sample1 open error\n");
			exit(1);
		}

		/*----------------------------------------------*/

		loop = 0;
		if(0 < (fd2 = open("sample2",O_RDONLY))){
			while( 0 < (rd_size = read(fd2, buf, BUFF_SIZE))){
				if(write(dev, buf, rd_size) < 0){
					perror("Write error");
					exit(1);
				}
				random = rand()%10000000;
				lseek(fd2, random, SEEK_SET);
				if(random >= 95536){
					lseek(fd2, 0, SEEK_SET);
				}
				if(loop % 1000 == 0){
				  //printf("sample 2, write : %d/65536\n", loop);
				}
				if(loop == 65537){
					break;
				}
				loop += 1;
			}
		}else{
			printf("sample2 open error\n");
			exit(1);
		}
		fsync(dev);
		write_end = clock();

		close(fd1);
		close(fd2);

		/*----------------------------------------------*/

		close(dev);

		/*----------------------------------------------*/

		if ((dev = open("/dev/sbulla", O_RDWR)) < 0) {
			printf("Device open error\n");
			exit(1);
		}

		/*----------------------------------------------*/

		read_start = clock();
		for(loop = 0; loop < SIZE; loop++){
			if(read(dev, buf, BUFF_SIZE) < 0){
				perror("Read error");
				exit(1);
			}
			if(loop % 1000 == 0){
				//printf("Read : %d/131072\n",loop);
			}
		}
		fsync(dev);
		read_end = clock();
		
		/*----------------------------------------------*/

		ioctl(dev, SBULL_PRINT_INFO);
		fsync(dev);
		close(dev);

		/*----------------------------------------------*/

		printf("Bench 2 Result ");
		switch (testloop) {
			case 0: printf("(without compression)\n"); break;
			case 1: printf("(with compression)\n"); break;
		}
		printf("	Write time : %.3lf ms\n",(float)(write_end - write_start)*1000/CLOCKS_PER_SEC);
		printf("	Read time : %.3lf ms\n",(float)(read_end-read_start)*1000/CLOCKS_PER_SEC);
	}
}
