#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static struct task_struct *thread_st;
// Function executed by kernel thread
static int thread_fn(void *unused)
{
	while (!kthread_should_stop()) {
		printk("Thread Running\n");
		ssleep(5);
	}
	return 0;
}
// Module Initialization
static int __init init_thread(void)
{
	printk(KERN_INFO "Creating Thread\n");
	thread_st = kthread_run(thread_fn, NULL, "SYS");
	if (thread_st) printk(KERN_INFO "Thread Created succesfully\n");
	else printk(KERN_ERR "Thread Creation failed\n");

	return 0;
}

// Module Exit
static void __exit cleanup_thread(void)
{
	printk(KERN_INFO "Cleaning Up\n");
	kthread_stop(thread_st);
	printk(KERN_INFO "Thread Stopping\n");
}

MODULE_LICENSE("GPL");
module_init(init_thread);
module_exit(cleanup_thread);
