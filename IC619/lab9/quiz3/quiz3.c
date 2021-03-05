#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/random.h>

#include "sbuf.h"

/* create an empty, bounded, shared FIFO buffer with n slots */
extern void sbuf_init(sbuf_t *sp, int n);

/* clean up buffer sp */
extern void sbuf_deinit(sbuf_t *sp);

/* insert item onto the rear of shared buffer sp */
extern void sbuf_insert(sbuf_t *sp, int item);

/* remove and return the first item from buffer sp */
extern int sbuf_remove(sbuf_t *sp);

/* if there is items in buffer, remove and return the first item from buffer sp. 
   if not, return -1, immediately */
extern int sbuf_tryremove(sbuf_t *sp);

#define ITEMS 30
#define SBUFSIZE 3
#define NUM_SBUF 4
#define NUM_THREADS 2

static struct task_struct **pthreads;
static struct task_struct **cthreads;
sbuf_t* sbufs = NULL;

static int producer (void* arg)
{
	// insert 0-14 integers to a queue
	// the queue is randomly seledted
	int i;
	for (i = 0; i < ITEMS / 2; i++) {
		int a;
		get_random_bytes(&a, sizeof(a));
		if (a < 0) a = -a;
		sbuf_insert(&sbufs[a % NUM_SBUF], i);
	}
	printk(KERN_INFO "Producer done\n");
	return 0;
}

static int consumer (void* arg)
{
	// remove 30 items and print each item
	int i, j;
	for (i = 0; i < ITEMS;) {
		for (j = 0; j < NUM_SBUF; j++) {
			int ret = sbuf_tryremove(&sbufs[j]);
			if (ret != -1) {
				printk(KERN_INFO "Consumer item %d = %d from queue %d\n", i, ret, j);
				i++;
				break;
			}
		}
	}
	printk(KERN_INFO "Consumer done\n");
	return 0;
}

static int simple_init (void)
{
	int i;
	// init 4 sbuf
	sbufs = kmalloc(NUM_SBUF * sizeof(sbuf_t), GFP_KERNEL);
	for (i = 0; i < NUM_SBUF; i++) {
		sbuf_init(&sbufs[i], SBUFSIZE);
	}

	// create 2 producer, 1 consumer
	pthreads = kmalloc(NUM_THREADS * sizeof(struct task_struct *), GFP_KERNEL);
	cthreads = kmalloc(1 * sizeof(struct task_struct *), GFP_KERNEL);
	for (i = 0; i < NUM_THREADS; i++) {
		pthreads[i] = kthread_run(producer, NULL, "SYS");
	}
	cthreads[0] = kthread_run(consumer, NULL, "SYS");
	return 0;
}

static void simple_exit(void)
{
	// deinit sbuf
	int i;
	for (i = 0; i < NUM_SBUF; i++) {
		sbuf_deinit(&sbufs[i]);
	}
	kfree(sbufs);

	kfree(pthreads);
	kfree(cthreads);
}

module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("KOO");
