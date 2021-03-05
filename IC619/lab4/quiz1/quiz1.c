#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int myshort = 1;
static int myint = 420;
static long int mylong = 9999;
static char *mystring = "System Programming";
static int myintArray[10] = {0,};

static int quiz1_init(void) {
	int i;
	printk(KERN_INFO "Loading Moudule\n");
	printk(KERN_INFO "Quiz 1\n=====================\n");
	printk(KERN_INFO "myshort is a short integer: %d\n", myshort);
	printk(KERN_INFO "myint is an integer: %d\n", myint);
	printk(KERN_INFO "mylong is a long integer: %d\n", mylong);
	printk(KERN_INFO "mystring is a string: %s\n", mystring);
	for (i = 0; i < 10; i++) { printk(KERN_INFO "myintArray[%d] = %d\n", i, myintArray[i]); }
	return 0;
}

static void quiz1_exit(void) {

	printk(KERN_INFO "Removing Module\n");
}

module_init( quiz1_init );
module_exit( quiz1_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Quiz1 Module");
MODULE_AUTHOR("Heeyong");
