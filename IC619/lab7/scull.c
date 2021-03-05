#include <linux/version.h>
#include <generated/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/uaccess.h>  /* copy_form_user() */
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/sort.h>

#include <asm/uaccess.h>	/* copy_*_user */

#include "scull.h"			/* local definitions */

/*
 * Our parameters which can be set at load time.
 */

int scull_major =   SCULL_MAJOR;
int scull_minor =   0;
int scull_nr_devs = SCULL_NR_DEVS;			/* number of bare scull devices */
int scull_quantum = SCULL_QUANTUM;
int scull_qset =    SCULL_QSET;
module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);

MODULE_AUTHOR("Alessandro Rubini, Jonathan Corbet");
MODULE_LICENSE("Dual BSD/GPL");

struct scull_dev *scull_devices;	/* allocated in scull_init_module */

static int scull_current_mode = 0;

static int scull_count_read = 0;
static int scull_count_write = 0;
static int scull_count_open = 0;
static int scull_count_close = 0;

/*
 * Empty out the scull device; must be called with the device
 * semaphore held.
 */
int scull_trim(struct scull_dev *dev) {
	struct scull_qset *next, *dptr;
	int qset = dev->qset;   /* "dev" is not-null */
	int i;

	for (dptr = dev->data; dptr; dptr = next) { /* all the list items */
		if (dptr->data) {
			for (i = 0; i < qset; i++)
				kfree(dptr->data[i]);
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}
	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	dev->data = NULL;
	return 0;
}

/*
 * Open and close
 */

// TODO: Complete
int scull_open(struct inode *inode, struct file *filp) {
	struct scull_dev *dev;
	dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = dev;

	if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {
		if (mutex_lock_interruptible(&dev->mut))
			return -ERESTARTSYS;
		scull_trim(dev);
		mutex_unlock(&dev->mut);
	}
	scull_count_open++;

    return 0;          /* success */
}

// TODO: Complete
int scull_release(struct inode *inode, struct file *filp) {
scull_count_close++;
    return 0;
}

/*
 * Follow the list
 */
struct scull_qset *scull_follow(struct scull_dev *dev, int n) {
	struct scull_qset *qs = dev->data;

        /* Allocate first qset explicitly if need be */
	if (! qs) {
		qs = dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
		if (qs == NULL)
			return NULL;  /* Never mind */
		memset(qs, 0, sizeof(struct scull_qset));
	}

	/* Then follow the list */
	while (n--) {
		if (!qs->next) {
			qs->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
			if (qs->next == NULL)
				return NULL;  /* Never mind */
			memset(qs->next, 0, sizeof(struct scull_qset));
		}
		qs = qs->next;
		continue;
	}
	return qs;
}

static int scull_compare(const void *p1, const void *p2) {
	int p1_int = *(const int *)(p1);
	int p2_int = *(const int *)(p2);

	if (p1_int < p2_int) return -1;
	if (p1_int > p2_int) return 0;
	return 0;
}

/*
 * Data management: read and write
 */
// TODO: Complete
ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
	struct scull_dev *dev = filp->private_data; 
	struct scull_qset *dptr;	/* the first listitem */
	int quantum = dev->quantum, qset = dev->qset;
	int itemsize = quantum * qset; /* how many bytes in the listitem */
	int item, s_pos, q_pos, rest;
	ssize_t retval = 0;
	int arr_sz, *array;

	if (mutex_lock_interruptible(&dev->mut))
		return -ERESTARTSYS;
scull_count_read++;
	if (*f_pos >= dev->size)
		goto out;
	if (*f_pos + count > dev->size)
		count = dev->size - *f_pos;

	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum;
	q_pos = rest % quantum;

	dptr = scull_follow(dev, item);

	if (dptr == NULL || !dptr->data || !dptr->data[s_pos])
		goto out;

	if (count > quantum - q_pos)
		count = quantum = q_pos;

	arr_sz = count >> 1;
	array = kmalloc(sizeof(int) * arr_sz, GFP_KERNEL); 

	if (scull_current_mode) {
		char* input = dptr->data[s_pos] + q_pos;
		int i;
		for (i = 0; i < count; i++) {
			if (i & 1) {
				char temp[3];
				int temp_int;
				temp[0] = input[i-1];
				temp[1] = input[i];
				temp[2] = '\0';
				kstrtoint(temp, 10, &temp_int); 
				array[i >> 1] = temp_int;
			}
		}

		sort(array, arr_sz, sizeof(int), scull_compare, NULL);

		memset(input, 0, sizeof(char) * count);
		for (i = 0; i < arr_sz; i++) {
			char temp[3];
			sprintf(temp, "%02d", array[i]);
			strcat(input, temp);
		}
	}

	if (copy_to_user(buf, dptr->data[s_pos] + q_pos, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

out:
	if (array) kfree(array);
	mutex_unlock(&dev->mut);
	return retval;
}

// TODO: complete
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
    struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum, qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t retval = -ENOMEM; /* value used in "goto out" statements */

	if (mutex_lock_interruptible(&dev->mut))
		return -ERESTARTSYS;

scull_count_write++;
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum;
	q_pos = rest % quantum;

	dptr = scull_follow(dev, item);
	if (dptr == NULL)
		goto out;
	if (!dptr->data) {
		dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
		if (!dptr->data)
			goto out;
		memset(dptr->data, 0, qset * sizeof(char *));
	}
	if (!dptr->data[s_pos]) {
		dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
		if (!dptr->data[s_pos])
			goto out;
	}
	if (count > quantum - q_pos)
		count = quantum - q_pos;

	if (copy_from_user(dptr->data[s_pos] + q_pos, buf, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

	if (dev->size < *f_pos)
		dev->size = *f_pos;

out:
	mutex_unlock(&dev->mut);
	return retval;
}

/*
 * The ioctl() implementation
 */
// TODO: Complete
long scull_ioctl(struct file *filp,
                 unsigned int cmd, unsigned long arg) {
	int err = 0;
	long retval = 0;

	if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;

	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;
	switch(cmd) {
	case SCULL_IOCRESET:
		scull_quantum = SCULL_QUANTUM;
		scull_qset = SCULL_QSET;
		break;
	case SCULL_CHANGE_MODE_SORT:
		printk(KERN_INFO "Change mode\n"
				"Sort mode\n");
		scull_current_mode = 1;
		break;
	case SCULL_CHANGE_MODE_NOT_SORT:
		printk(KERN_INFO "Change mode\n"
				"Not sort mode\n");
		scull_current_mode = 0; 
		break;
	case SCULL_GET_CURRENT_MODE:
		{
		char temp[2];
		sprintf(temp, "%d", scull_current_mode); 
		copy_to_user((char*)arg, temp, 2);
		break;
		}

	case SCULL_PRINT_INFORMATION:
		printk(KERN_INFO "Device Info\n"
				"Read: %d\n"
				"Write: %d\n"
				"Open: %d\n"
				"Close: %d\n",
				scull_count_read,
				scull_count_write,
				scull_count_open,
				scull_count_close);
				 
		break;
	default:  
		return -ENOTTY;
	} 

	return retval;
}

/*
 * The "extended" operations -- only seek
 */
loff_t scull_llseek(struct file *filp, loff_t off, int whence) {
	struct scull_dev *dev = filp->private_data;
	loff_t newpos;

	switch(whence) {
	  case 0: /* SEEK_SET */
		newpos = off;
		break;

	  case 1: /* SEEK_CUR */
		newpos = filp->f_pos + off;
		break;

	  case 2: /* SEEK_END */
		newpos = dev->size + off;
		break;

	  default: /* can't happen */
		return -EINVAL;
	}
	if (newpos < 0) return -EINVAL;
	filp->f_pos = newpos;
	return newpos;
}

// TODO: Complete
struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	.llseek = scull_llseek,
	.read = scull_read,
	.write = scull_write,
	.unlocked_ioctl = scull_ioctl,
	.open = scull_open,
	.release = scull_release,
};

/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
 */
void scull_cleanup_module(void) {
	int i;
	dev_t devno = MKDEV(scull_major, scull_minor);

	/* Get rid of our char dev entries */
	if (scull_devices) {
		for (i = 0; i < scull_nr_devs; i++) {
			scull_trim(scull_devices + i);
			cdev_del(&scull_devices[i].cdev);
		}
		kfree(scull_devices);
	}

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, scull_nr_devs);
}

/*
 * Set up the char_dev structure for this device.
 */
// TODO: Complete
static void scull_setup_cdev(struct scull_dev *dev, int index) {
	int err, devno = MKDEV(scull_major, scull_minor + index);

	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scull_fops;
	err = cdev_add(&dev->cdev, devno, 1);

	if (err) printk(KERN_NOTICE "Error %d adding scull %d", err, index);
}

// TODO: Complete
int scull_init_module(void) {
	int result = 0, i;
	dev_t dev = 0;

	if (scull_major) {
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, "scull");
	} else {
		result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
		scull_major = MAJOR(dev);
	}

	if (result < 0) {
		printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
		goto fail;
	}

	scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
	if (!scull_devices) {
		result = -ENOMEM;
		goto fail;
	}
	memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));

	for (i = 0; i < scull_nr_devs; i++) {
		scull_devices[i].quantum = scull_quantum;
		scull_devices[i].qset = scull_qset;
		mutex_init(&scull_devices[i].mut);
		scull_setup_cdev(&scull_devices[i], i);
	}

	dev = MKDEV(scull_major, scull_minor + scull_nr_devs);

	return 0; /* succeed */

  fail:
	scull_cleanup_module();
	return result;
}

module_init(scull_init_module);
module_exit(scull_cleanup_module);
