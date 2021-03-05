#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>

int main(){
    int fd;
    int ret;
    char buf[256];

    fd = open("/dev/scull0", O_RDWR);
    if (fd < 0){
        perror("open: ");
		return -1;
    }
    ret = ioctl(fd, 0, buf);
    printf("ret : %d\n",ret);
    close(fd);
    return 0;
}
