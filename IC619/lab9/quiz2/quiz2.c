#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/slab.h>

#include "sbuf.h"

/* create an empty, bounded, shared FIFO buffer with n slots */
extern void sbuf_init(sbuf_t *sp, int n);

/* clean up buffer sp */
extern void sbuf_deinit(sbuf_t *sp);

/* insert item onto the rear of shared buffer sp */
extern void sbuf_insert(sbuf_t *sp, int item);

/* remove and return the first item from buffer sp */
extern int sbuf_remove(sbuf_t *sp);

#define ITEMS 30
#define SBUFSIZE 3
#define NUM_SBUF 1
#define NUM_THREADS 1

static struct task_struct **pthreads;
static struct task_struct **cthreads;
sbuf_t* sbufs = NULL;

static int producer (void* arg)
{
	int i;
	for (i = 0; i < ITEMS; i++) {
		sbuf_insert(sbufs, i);
	}
	printk(KERN_INFO "Producer done\n");
	return 0;
}

static int consumer (void* arg)
{
	// remove 30 items and print each item
	int i, val;
	for (i = 0; i < ITEMS; i++) {
		val = sbuf_remove(sbufs);
		printk(KERN_INFO "consumer item %d = %d\n", i, val);
	}
	printk(KERN_INFO "Consumer done\n");
	return 0;
}

static int simple_init (void)
{
	pthreads = kmalloc(NUM_THREADS * sizeof(struct task_struct *), GFP_KERNEL);
	cthreads = kmalloc(NUM_THREADS * sizeof(struct task_struct *), GFP_KERNEL);

	// init 1 sbuf
	sbufs = kmalloc(NUM_SBUF * sizeof(sbuf_t), GFP_KERNEL);
	sbuf_init(sbufs, SBUFSIZE);
	// create 1 producer, 1 consumer
	pthreads[0] = kthread_run(producer, NULL, "SYS");
	cthreads[0] = kthread_run(consumer, NULL, "SYS");

	return 0;
}

static void simple_exit(void)
{
	// deinit sbuf
	sbuf_deinit(sbufs);

	kfree(sbufs);
	kfree(pthreads);
	kfree(cthreads);
}

module_init( simple_init ); 
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("KOO");
