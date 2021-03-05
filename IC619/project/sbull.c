#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/timer.h>
#include <linux/types.h>	/* size_t */
#include <linux/kdev_t.h>
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/bio.h>

#include <linux/lz4.h>
#include <linux/types.h>

#include "sbull.h"

#define SBULL_MINORS 16
#define KERNEL_SECTOR_SIZE 512 // kernel's sector size is fixed on 512 due to old HDD.

// licence
MODULE_LICENSE("Dual BSD/GPL");

static int sbull_major = 0;

// device has 1024 * 1024 * 512 bytes
static const int hardsect_size = 512;
static const unsigned long nsectors = 2 * 1024 * 1024;

// total 1 device
static const int ndevices = 1;

// each device
struct sbull_dev {
	unsigned long long size; // allocated memory size 
	int users;
	char *data; // memory location 
	char *lz4mem;
	spinlock_t lock;

	struct request_queue *queue;
	struct gendisk *gd;
};

// device list
static struct sbull_dev *Devices = NULL;

static atomic_t ORIGINAL_SIZE = ATOMIC_INIT(0),
				COMPRESSED_SIZE = ATOMIC_INIT(0),
				IO_READ_COUNT = ATOMIC_INIT(0),
				IO_WRITE_COUNT = ATOMIC_INIT(0);
static int COMPRESSION_ENABLED = 0;

static void sbull_transfer(
		struct sbull_dev *dev,
		unsigned long sector,
		unsigned long nsect,
		char *buffer,
		int write)
{
	unsigned long offset = sector * KERNEL_SECTOR_SIZE;
	unsigned long nbytes = nsect * KERNEL_SECTOR_SIZE;
	unsigned long ret;

	if ((offset + nbytes) > dev->size) {
		printk(KERN_NOTICE "Beyond-end write (%ld %ld)\n", offset, nbytes);
		return;
	}
	if (write) {
		//ORIGINAL_SIZE += nbytes;
		//IO_WRITE_COUNT++;
		atomic_add(nbytes, &ORIGINAL_SIZE);
		atomic_inc(&IO_WRITE_COUNT);

		if (COMPRESSION_ENABLED) {
			ret = LZ4_compress_default(buffer, dev->data + offset, nbytes, LZ4_compressBound(nbytes), dev->lz4mem);
			if (ret <= 0) {
				printk(KERN_WARNING "Compression failed at %ld\n", offset);
			}
			atomic_add(ret, &COMPRESSED_SIZE);
			//COMPRESSED_SIZE += ret;
		} else {
			atomic_add(nbytes, &COMPRESSED_SIZE);
			//COMPRESSED_SIZE += nbytes; 
			memcpy(dev->data + offset, buffer, nbytes);
		}
	} else {
		atomic_inc(&IO_READ_COUNT);
		//IO_READ_COUNT++;

		if (COMPRESSION_ENABLED) {
			ret = LZ4_decompress_safe(dev->data + offset, buffer, LZ4_compressBound(nbytes), nbytes);
			if (ret <= 0) {
				printk(KERN_WARNING "Decompression failed at %ld\n", offset);
			}
		} else {
			memcpy(buffer, dev->data + offset, nbytes);
		}
	}
}

static int sbull_xfer_bio(struct sbull_dev *dev, struct bio *bio)
{
	struct bvec_iter i;
	struct bio_vec bvec;
	sector_t sector = bio->bi_iter.bi_sector;
	bio_for_each_segment(bvec, bio, i) {
		char *buffer = __bio_kmap_atomic(bio, i);

		sbull_transfer(dev, sector, bio_cur_bytes(bio) >> 9,
					buffer, bio_data_dir(bio) == WRITE);

		sector += bio_cur_bytes(bio) >> 9;
		__bio_kunmap_atomic(buffer);
	}
	return 0;
}

static void sbull_make_request(struct request_queue *q, struct bio *bio)
{
	struct sbull_dev *dev = q->queuedata;
	int status;

	status = sbull_xfer_bio(dev, bio);
    bio_endio(bio);	
}

static int sbull_open(struct block_device *bdev, fmode_t mode)
{
    struct sbull_dev *dev = bdev->bd_disk->private_data;
	spin_lock(&dev->lock);
	if (!dev->users) check_disk_change(bdev);
	dev->users++;
	spin_unlock(&dev->lock);
    return 0;
}

static void sbull_release(struct gendisk *disk, fmode_t mode)
{
	struct sbull_dev *dev = disk->private_data;
	spin_lock(&dev->lock);
	dev->users--;
	spin_unlock(&dev->lock);
}

static int sbull_ioctl(
		struct block_device *bdev, 
		fmode_t mode, 
		unsigned int cmd, 
		unsigned long arg)
{
    int ret = 0;
	switch (cmd) {
		case SBULL_PRINT_INFO:
			switch (COMPRESSION_ENABLED) {
				case 0: printk(KERN_INFO "Compression disabled\n"); break;
				case 1: printk(KERN_INFO "Compression enabled\n"); break;
			}
			printk(KERN_INFO "    Original size: %4dMB\n", atomic_read(&ORIGINAL_SIZE) / (1024 * 1024));
			printk(KERN_INFO "  Compressed size: %4dMB\n", atomic_read(&COMPRESSED_SIZE) / (1024 * 1024));
			printk(KERN_INFO "        I/O count: Read  %7d\n", atomic_read(&IO_READ_COUNT));
			printk(KERN_INFO "                   Write %7d\n", atomic_read(&IO_WRITE_COUNT));
			break;
		case SBULL_CLEAR_INFO:
			atomic_set(&ORIGINAL_SIZE, 0);
			atomic_set(&COMPRESSED_SIZE, 0);
			atomic_set(&IO_READ_COUNT, 0);
			atomic_set(&IO_WRITE_COUNT, 0);
			break;
		case SBULL_ENABLE_COMPRESS:
			COMPRESSION_ENABLED = 1;
			break;
		case SBULL_DISABLE_COMPRESS:
			COMPRESSION_ENABLED = 0;
			break;
		default:
			ret = -ENOTTY;
			break;
	}
	return ret;
}

static struct block_device_operations sbull_ops = {
	.owner = THIS_MODULE,
	.open = sbull_open,
	.release = sbull_release,
	.ioctl = sbull_ioctl
};

static void setup_device(struct sbull_dev *dev, const int which)
{
	// get memory
	memset(dev, 0, sizeof(struct sbull_dev));
	dev->size = nsectors * hardsect_size;
	dev->users = 0;
	dev->data = vmalloc(dev->size);
	if (dev->data == NULL) {
		printk(KERN_NOTICE "vmalloc failure. (dev->data\n");
		return;
	}
	dev->lz4mem = vmalloc(LZ4_MEM_COMPRESS);
	if (dev->lz4mem == NULL) {
		printk(KERN_NOTICE "vmalloc failure. (dev->lz4mem\n");
		return;
	}
	spin_lock_init(&dev->lock);

	// init request_queue struct
    dev->queue = blk_alloc_queue(GFP_KERNEL);
    if (dev->queue == NULL) {
        goto out_vfree;
	}
    blk_queue_make_request(dev->queue, (make_request_fn *)sbull_make_request);		
    dev->queue->queuedata = dev;

	// init gendisk struct
	dev->gd = alloc_disk(SBULL_MINORS);
	if (!dev->gd) {
		printk(KERN_NOTICE "alloc_disk failure\n");
		goto out_vfree;
	}
	dev->gd->major = sbull_major;
	dev->gd->first_minor = which * SBULL_MINORS;
	dev->gd->fops = &sbull_ops;
	dev->gd->queue = dev->queue;
	dev->gd->private_data = dev;
	snprintf(dev->gd->disk_name, 32, "sbull%c", which + 'a');
	set_capacity(dev->gd, nsectors * (hardsect_size / KERNEL_SECTOR_SIZE));
	add_disk(dev->gd);
    
    return;

out_vfree:
    if (dev->data) vfree(dev->data);
	if (dev->lz4mem) vfree(dev->lz4mem);
}

static int __init sbull_init(void)
{
	int i;
	
	// Major number
	sbull_major = register_blkdev(sbull_major, "sbull");
	if (sbull_major <= 0) {
		printk(KERN_WARNING "sbull: unable to get major number\n");
		return -EBUSY;
	}

	// memalloc each device
	Devices = kmalloc(ndevices * sizeof(struct sbull_dev), GFP_KERNEL);
	if (Devices == NULL) {
		unregister_blkdev(sbull_major, "sbd");
		return -ENOMEM;
	}

	for (i = 0; i < ndevices; i++) {
		setup_device(Devices + i, i);
	}

    return 0;
}

static void __exit sbull_exit(void)
{
	int i;

	for (i = 0; i < ndevices; i++) {
		struct sbull_dev *dev = Devices + i;

		if (dev->gd) {
			del_gendisk(dev->gd);
			put_disk(dev->gd);
		}
		if (dev->queue) kobject_put(&dev->queue->kobj);
		if (dev->data) vfree(dev->data);
		if (dev->lz4mem) vfree(dev->lz4mem);
	}
	unregister_blkdev(sbull_major, "sbull");
	kfree(Devices);
}
	
module_init(sbull_init);
module_exit(sbull_exit);
