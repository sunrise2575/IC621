#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>

int main(){
    int fd;
    int ret;
    int buf;

    fd = open("/dev/sbulla", O_RDWR);
    if (fd < 0){
        perror("open: ");
		return -1;
    }
    ret = ioctl(fd, 0, buf);
    printf("ret : %d\n",ret);
    close(fd);
    return 0;
}
