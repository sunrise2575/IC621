#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include "quiz3_init.h"
#include <linux/random.h>

my_node *data_node;
struct rb_root my_root = RB_ROOT;

my_node* rb_search(struct rb_root *root, char string){
	struct rb_node* node = root->rb_node;

	while (node) {
		my_node *data = container_of(node, my_node, node);
		//my_node *data = rb_entry(node, my_node, node);
		//int result = strcmp(&string, &data->keystring);
		int result = string - data->keystring;
		if (result < 0) { node = node->rb_left; }
		else if (result > 0) { node = node->rb_right; }
		else { return data; }
	}
	return NULL;
}

int rb_insert(struct rb_root *root, my_node *data){
	struct rb_node **new = &(root->rb_node), *parent = NULL;
	while (*new) {
		my_node *this = container_of(*new, my_node, node);
		//int result = strcmp(&data->keystring, &this->keystring);
		int result = data->keystring - this->keystring;
		parent = *new;
		if (result < 0) { new = &((*new)->rb_left); }
		else if (result > 0) { new = &((*new)->rb_right); }
		else { return FALSE; }
	}
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);

	return TRUE;
}

void create(){
    int ran = 0, loop = 0;
    data_node = (my_node *)kmalloc(sizeof(my_node)*NODE_SIZE, GFP_KERNEL);
    for(loop = 0; loop < NODE_SIZE; loop++){
        get_random_bytes(&ran, sizeof(int)-1);
        data_node[loop].keystring = 'A' + (ran % 26);
    }
}

void destroy(){
    kfree(data_node);
}
