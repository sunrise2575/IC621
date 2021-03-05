#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
	int fd;
	char *buf1 = "1920170001087899";
	char *buf2;

	if((fd = open("/dev/scull0",O_RDWR)) < 0) {
	    perror("open error");
		exit(1);
	}
	
	if(write(fd, buf1, 16) < 0) {
		perror("write error");
		exit(1);
	}

	if((fd = open("/dev/scull0",O_RDWR)) < 0) {
	    perror("open error");
		exit(1);
	}

	buf2 = (char*)calloc(1,17);
	
	if(read(fd, buf2, 16) < 0) {
		perror("read error");
		exit(1);
	}
	
	printf("input : %s\n", buf1);
	printf("ouput : %s\n", buf2);
	
	close(fd);

	return 0;
}
