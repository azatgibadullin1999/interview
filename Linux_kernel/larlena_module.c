#include <linux/module.h>
#include <linux/kernel.h> // printk()
#include <linux/types.h> // dev_t
#include <linux/fs.h> // register_chardev_region() ...
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>

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

struct ft_scull_qset {
	void			**data;
	struct ft_scull_qset	*next;
} ;

struct ft_scull_dev {
	struct ft_scull_qset	*data;
	unsigned int		quantum;
	unsigned int		qset;
	unsigned long		size;
	unsigned int		access_key;
	struct mutex		mutex;
	struct cdev		cdev;
} ;

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

static struct ft_scull_qset	*ft_follow(struct ft_scull_dev *dev, int n) {
	struct ft_scull_qset	*qs = dev->data;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	if (!qs) {
		if (!(qs = dev->data = kmalloc(sizeof(*qs), GFP_KERNEL))) {
			return NULL;
		}
		memset(qs, 0, sizeof(*qs));
	}
	while (n--) {
		if (!qs->next) {
			qs->next = kmalloc(sizeof(*qs), GFP_KERNEL);
			if (qs->next == NULL) {
				return NULL;
			}
			memset(qs->next, 0, sizeof(*qs));
		}
		qs = qs->next;
	}
	printk(KERN_ALERT "End the %s function\n", __FUNCTION__);
	return qs;
}

static ssize_t	ft_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
	struct ft_scull_dev	*dev = filp->private_data;
	struct ft_scull_qset	*dptr;
	int	quantum = dev->quantum, qset = dev->qset;
	int	itemsize = quantum * qset;
	int	item, s_pos, q_pos, rest;
	ssize_t	ret_value = -ENOMEM;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	if (mutex_lock_interruptible(&dev->mutex)) {
		return -ERESTARTSYS;
	}
	printk(KERN_ALERT "mutex is ok\n");
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum;
	q_pos = rest % quantum;

	dptr = ft_follow(dev, item);
	printk(KERN_ALERT "ft_follow is ok\n");
	if (dptr == NULL) {
		goto ft_write_out;
	}
	if (!dptr->data) {
		dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
		if (!dptr->data) {
			goto ft_write_out;
		}
		memset(dptr->data, 0, qset * sizeof(char *));
	}
	if (!dptr->data[s_pos]) {
		dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
		if (!dptr->data[s_pos]) {
			goto ft_write_out;
		}
	}
	if (count > quantum - q_pos) {
		count = quantum - q_pos;
	}
	if (copy_from_user(dptr->data[s_pos] + q_pos, buf, count)) {
		ret_value = -EFAULT;
		goto ft_write_out;
	}
	*f_pos += count;
	ret_value = count;

	if (dev->size < *f_pos) {
		dev->size = *f_pos; 
	}
ft_write_out :
	mutex_unlock(&dev->mutex);
	printk(KERN_ALERT "End the %s function\n", __FUNCTION__);
	return ret_value;
}

ssize_t	ft_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
	struct ft_scull_dev	*dev = filp->private_data;
	struct ft_scull_qset	*dptr;
	int	quantum = dev->quantum, qset = dev->qset;
	int	itemsize = quantum * qset;
	int	item, s_pos, q_pos, rest;
	ssize_t	ret_value = 0;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	if (mutex_lock_interruptible(&dev->mutex)) {
		return -ERESTARTSYS;
	}
	if (*f_pos >= dev->size) {
		goto ft_read_out;
	}
	if (*f_pos + count > dev->size) {
		count = dev->size - *f_pos;
	}

	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum;
	q_pos = rest % quantum;

	dptr = ft_follow(dev, item);

	if (dptr == NULL ||  !dptr->data || !dptr->data[s_pos]) {
		goto ft_read_out;
	}
	if (count > quantum - q_pos) {
		count = quantum - q_pos;
	}
	if (copy_to_user(buf, dptr->data[s_pos] + q_pos, count)) {
		ret_value = -EFAULT;
		goto ft_read_out;
	}
	*f_pos += count;
	ret_value = count;

ft_read_out :
	mutex_unlock(&dev->mutex);
	printk(KERN_ALERT "End the %s function\n", __FUNCTION__);
	return ret_value;
}

static int	ft_trim(struct ft_scull_dev *dev) {
	struct ft_scull_qset *next, *dptr = dev->data;
	int	qset = dev->qset;
	size_t	i;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	while (dptr) {
		if (dptr->data) {
			for (i = 0; i < qset; ++i) {
				kfree(dptr->data[i]);
			}
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
		kfree(dptr);
		dptr = next;
	}
	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	dev->data = NULL;
	printk(KERN_ALERT "End the %s function\n", __FUNCTION__);
	return 0;
}

int	ft_open(struct inode *inode, struct file *filp) {
	struct ft_scull_dev	*dev;


	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	dev = container_of(inode->i_cdev, struct ft_scull_dev, cdev);
	filp->private_data = dev;
	if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {
		if (mutex_lock_interruptible(&dev->mutex)) {
			return -ERESTARTSYS;
		}
		ft_trim(dev);
		mutex_unlock(&dev->mutex);
	}
	printk(KERN_ALERT "End the %s function\n", __FUNCTION__);
	return 0;
}

int	ft_release(struct inode *inode, struct file *filp) {
	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	printk(KERN_ALERT "End the %s function\n", __FUNCTION__);
	return 0;
}

static struct file_operations	scull_fops = {
	.owner		= THIS_MODULE,
	// .llseek		= ft_llseek,
	.read		= ft_read,
	.write		= ft_write,
	// .ioctl		= ft_ioctl,
	.open		= ft_open,
	.release	= ft_release,
} ;

static void __init ft_scull_setup_cdev(struct ft_scull_dev *dev, int index) {
	dev_t	devno = MKDEV(scull_major, scull_minor + index);

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
		mutex_init(&scull_devices[i].mutex);
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
		ft_trim(&scull_devices[i]);
		cdev_del(&scull_devices[i].cdev);
		mutex_destroy(&scull_devices[i].mutex);
		++i;
	}
	kfree(scull_devices);
	printk(KERN_ALERT "End the %s function\n", __FUNCTION__);
}

module_init(ft_larlenas_driver_init);
module_exit(ft_larlenas_driver_exit);

