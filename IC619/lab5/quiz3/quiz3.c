#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/hashtable.h>
#include <linux/rbtree.h>
#include "quiz3_init.h"

int kernel_init(void){
	int loop, sorted_sz = 0;
	struct rb_node *node;
	char sorted[NODE_SIZE] = {0,};

	printk(KERN_INFO "Quiz 3: %s\n", __PRETTY_FUNCTION__);
	create();
	// insert
	for(loop = 0; loop < NODE_SIZE; loop++){
		rb_insert(&my_root, &data_node[loop]);
	}
	
	// inorder traversal
	printk(KERN_INFO "Inorder Traversal\n");
	for (node = rb_first(&my_root); node; node = rb_next(node)) {
		char c = rb_entry(node, my_node, node)->keystring;
		printk(KERN_INFO "key=%c\n", c);
		sorted[sorted_sz] = c;
		sorted_sz++;
	}

	// pop and remove
	printk(KERN_INFO "Pop & Remove\n");
	for(loop = 0; loop < sorted_sz; loop++){
		my_node *data = rb_search(&my_root, sorted[loop]);
		printk("key=%c\n", data->keystring);
		rb_erase(&data->node, &my_root);
	}
	return 0;
}

void kernel_exit(void){
	destroy();
}

module_init(kernel_init);
module_exit(kernel_exit);

MODULE_LICENSE("GPL");
