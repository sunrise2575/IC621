#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
	int fd;
	char *buf1 = "hello";
	char *buf2;

	if((fd = open("/dev/scull0",O_RDWR)) < 0) {
	    perror("open error");
		exit(1);
	}
	
	if(write(fd, buf1, 5) < 0) {
		perror("write error");
		exit(1);
	}

	if((fd = open("/dev/scull0",O_RDWR)) < 0) {
	    perror("open error");
		exit(1);
	}

	buf2 = (char*)calloc(1,6);
	
	if(read(fd, buf2, 5) < 0) {
		perror("read error");
		exit(1);
	}
	
	printf("buf2 : %s\n", buf2);
	
	close(fd);

	return 0;
}
