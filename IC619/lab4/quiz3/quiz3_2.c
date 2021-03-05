#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

extern int countOne(int);

static int quiz3_2_init(void) {
	printk(KERN_INFO "Loading Moudule\n");
	printk(KERN_INFO "Quiz 3_2\n=====================\n");
	printk(KERN_INFO "countOne(1293) = %d\n", countOne(1293));
	printk(KERN_INFO "countOne(592394) = %d\n", countOne(592394));
	return 0;
}

static void quiz3_2_exit(void) {

	printk(KERN_INFO "Removing Module\n");
}

module_init( quiz3_2_init );
module_exit( quiz3_2_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Quiz3_2 Module");
MODULE_AUTHOR("18SYS");
