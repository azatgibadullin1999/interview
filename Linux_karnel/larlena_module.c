#include <linux/module.h>
#include <linux/kernel.h> // printk()
#include <linux/types.h> // dev_t
#include <linux/fs.h> // register_chardev_region() ...
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>

// TESTING LIBRARY 
	// #include <linux/sched.h>
	// #include <asm/current.h>
//
MODULE_LICENSE("GPL");

struct ft_scull_dev {
	// int			quantum;
	// int			qset;
	unsigned long		size;
	unsigned int		access_key;
	struct semaphore	sem;
	struct cdev		cdev;
};

static struct file_operations	scull_fops; /* = {
	.owner		= THIS_MODULE,
	.llseek		= ft_llseek,
	.read		= ft_read,
	.write		= ft_write,
	.ioctl		= ft_ioctl,
	.open		= ft_open,
	.release	= ft_release,
} ;*/
static struct file		*filp;
static struct ft_scull_dev	scull_devices;

static unsigned int	scull_major = 0;
static unsigned int	scull_minor = 0;
static unsigned int	scull_nr_devs = 1;
// module_param(scull_major, ulong, 0644);
// module_param(scull_minor, ulong, 0644);
// module_param(scull_nr_devs, ulong, 0644);

static void __init ft_scull_setup_cdev(struct ft_scull_dev *dev, int index) {
	int	devno = MKDEV(scull_major, scull_minor + index);

	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scull_fops;
	if (cdev_add(&dev->cdev, devno, 1)) {
		printk(KERN_WARNING "cdev_add() failed\n");
	}
}

static int __init ft_larlenas_driver_init(void) {
	dev_t	dev = 0;
	int 	ret_value = 0;

	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	if (scull_major) {
		dev = MKDEV(scull_major, scull_minor);
		ret_value = register_chrdev_region(dev, scull_nr_devs,"ft_scull");
	} else {
		ret_value = alloc_chrdev_region(&dev, 0, scull_nr_devs, "ft_scull");
		scull_major = MAJOR(dev);
	}
	if (ret_value < 0) {
		printk(KERN_WARNING "some_chrdev_region failed\n");
		goto some_chardev_region_failed;
	}
	memset(&scull_devices, 0, sizeof(scull_devices));
	ft_scull_setup_cdev(&scull_devices, 0);

	return 0;
some_chardev_region_failed :
	return ret_value;
}

static void __exit ft_larlenas_driver_exit(void) {
	dev_t	devno = MKDEV(scull_major, scull_minor);

	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	cdev_del(&scull_devices.cdev);
	unregister_chrdev_region(devno, scull_nr_devs);
}

module_init(ft_larlenas_driver_init);
module_exit(ft_larlenas_driver_exit);

