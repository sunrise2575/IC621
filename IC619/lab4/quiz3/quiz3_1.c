#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int countOne(int num) {
	int count = 0;
	while (num > 0) {
		if (num & 0x1) { count++; }
		num >>= 1;
	}
	return count;
}

EXPORT_SYMBOL(countOne);

static int quiz3_1_init(void) {
	printk(KERN_INFO "Loading Moudule\n");
	printk(KERN_INFO "Quiz 3_1\n=====================\n");
	return 0;
}

static void quiz3_1_exit(void) {

	printk(KERN_INFO "Removing Module\n");
}

module_init( quiz3_1_init );
module_exit( quiz3_1_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Quiz3_1 Module");
MODULE_AUTHOR("18SYS");
