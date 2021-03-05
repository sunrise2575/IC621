#include <stdio.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#define SCULL_IOC_MAGIC  'k'
#define SCULL_CHANGE_MODE_SORT _IO(SCULL_IOC_MAGIC, 1)
#define SCULL_CHANGE_MODE_NOT_SORT _IO(SCULL_IOC_MAGIC, 2)
#define SCULL_GET_CURRENT_MODE _IOR(SCULL_IOC_MAGIC, 3, int)
#define SCULL_PRINT_INFORMATION _IO(SCULL_IOC_MAGIC, 4)

int main() {
	int fd, ret;

	if ((fd = open("/dev/scull0", O_RDWR)) < 0) {
		perror("open error"); exit(1); }

	if ((ret = ioctl(fd, SCULL_PRINT_INFORMATION)) < 0) {
		printf("ret: %d\n", ret);
		perror("open error"); exit(1); }

	close(fd);
	printf("ret: %d\n", ret);

	return 0;
}
