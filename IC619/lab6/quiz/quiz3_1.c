#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static void countOne(int n) {
	int cnt=0, num;
	num = n;
	while (num) {
		if (num & 1) cnt++;
		num >>= 1;
	}
	printk(KERN_INFO "countOne(%d) = %d\n", n, cnt);
}

static int simple_init(void) // 모듈이 생성될 때의 함수
{
	printk(KERN_INFO "Loading Moudule\n");
	printk(KERN_INFO "Quiz 3_1\n=====================\n");
	return 0;
}

static void simple_exit(void) //모듈이 삭제될 때의 함수
{

	printk(KERN_INFO "Removing Module\n"); // 모듈삭제를 알리는 커널메세지 출력
}

module_init( simple_init ); // 모듈 생성될 때 simpel_init 함수 호출
module_exit( simple_exit ); // 모듈 생성될 때 simpel_exit 함수 호출

EXPORT_SYMBOL(countOne);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("KOO");
