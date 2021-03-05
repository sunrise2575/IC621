#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>

int main(){
    int fd;
    int ret;
    int key = 5;

    fd = open("/dev/sbulla", O_RDWR);
    if (fd < 0){
        perror("open: ");
		return -1;
    }
    ret = ioctl(fd, 1, &key);
    printf("ret : %d\n",ret);
    close(fd);
    return 0;
}
