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
#ifndef SCULL_QUANTUM
# define SCULL_QUANTUM 4000
#endif

#ifndef SCULL_QSET
# define SCULL_QSET  1000
#endif

#ifndef SCULL_NR_DEVS
# define SCULL_NR_DEVS 1
#endif

#ifndef SCULL_MAJOR
# define SCULL_MAJOR 0
#endif

MODULE_LICENSE("GPL");

struct ft_scull_dev {
	unsigned int		quantum;
	unsigned int		qset;
	unsigned long		size;
	unsigned int		access_key;
	struct semaphore	sem;
	struct cdev		cdev;
} ;

static struct file		*filp;
static struct ft_scull_dev	*scull_devices;

static unsigned int	scull_major = SCULL_MAJOR;
static unsigned int	scull_minor = 0;
static unsigned int	scull_nr_devs = SCULL_NR_DEVS;
static unsigned int	scull_quantum = SCULL_QUANTUM;
static unsigned int	scull_qset = SCULL_QSET;
module_param(scull_major, uint, S_IRUGO);
module_param(scull_nr_devs, uint, S_IRUGO);
module_param(scull_quantum, uint, S_IRUGO);
module_param(scull_qset, uint, S_IRUGO);


static struct file_operations	scull_fops; /* = {
	.owner		= THIS_MODULE,
	.llseek		= ft_llseek,
	.read		= ft_read,
	.write		= ft_write,
	.ioctl		= ft_ioctl,
	.open		= ft_open,
	.release	= ft_release,
} ;*/

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
	size_t	i = 0;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	if (scull_major) {
		dev = MKDEV(scull_major, scull_minor);
		ret_value = register_chrdev_region(dev, scull_nr_devs,"ft_scull");
	} else {
		ret_value = alloc_chrdev_region(&dev, 0, scull_nr_devs, "ft_scull");
		scull_major = MAJOR(dev);
	}
	if (ret_value < 0) {
		printk(KERN_WARNING "some_chrdev_region failed\n");
		goto init_some_chardev_region_failed;
	}
	if (!(scull_devices = kmalloc(scull_nr_devs * sizeof(*scull_devices), GFP_KERNEL))) {
		printk(KERN_WARNING "kmalloc() faild\n");
		goto init_kmalloc_faild;
	}
	memset(scull_devices, 0, scull_nr_devs * sizeof(*scull_devices));
	while (i < scull_nr_devs) {
		scull_devices[i].qset = scull_qset;
		scull_devices[i].quantum = scull_quantum;
		ft_scull_setup_cdev(&scull_devices[i], 0);
		++i;
	}
	printk(KERN_ALERT "End the %s function\n", __FUNCTION__);

	return 0;
init_kmalloc_faild :
	unregister_chrdev_region(dev, scull_nr_devs);
init_some_chardev_region_failed :
	return ret_value;
}

static void __exit ft_larlenas_driver_exit(void) {
	dev_t	devno = MKDEV(scull_major, scull_minor);
	size_t	i = 0;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	unregister_chrdev_region(devno, scull_nr_devs);
	while (i < scull_nr_devs) {
		cdev_del(&scull_devices[i].cdev);
		++i;
	}
	kfree(scull_devices);
	printk(KERN_ALERT "End the %s function\n", __FUNCTION__);
}

module_init(ft_larlenas_driver_init);
module_exit(ft_larlenas_driver_exit);

