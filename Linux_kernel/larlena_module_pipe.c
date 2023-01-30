#include "larlena_module.h"

MODULE_LICENSE("GPL");

struct ft_pipe {
	wait_queue_head_t	inq, outq;
	int			buffersize;
	char			*buffer, *p_end;
	char			*p_read, *p_write;
	int			nreaders, nwriters;
	struct fasync_struct	*async_queue;
	struct mutex		mutex;
	struct cdev 		cdev;
} ;

static unsigned int	ft_pipe_major = PIPE_MAJOR;
static unsigned int	ft_pipe_minor = 0;
static unsigned int	ft_pipe_nr_devs = PIPE_NR_DEVS;
static unsigned int	ft_pipe_buffer = PIPE_BUFFER;

module_param(ft_pipe_major, uint, S_IRUGO);
module_param(ft_pipe_nr_devs, uint, S_IRUGO);
module_param(ft_pipe_buffer, uint, S_IRUGO);

dev_t			ft_pipe_devno;
static struct ft_pipe	*ft_pipe_devices;

static long	ft_pipe_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
	if (_IOC_TYPE(cmd) != FT_PIPE_IOC_MAGIC) {
		return -ENOTTY;
	}
	if (_IOC_NR(cmd) > FT_IOC_MAXNR) {
		return -ENOTTY;
	}
	if ((	_IOC_DIR(cmd) & _IOC_READ || _IOC_DIR(cmd) & _IOC_WRITE) &&
		!access_ok((void __user *)arg, _IOC_SIZE(cmd))) {
			return -EFAULT;
	}

	if (cmd == FT_PIPE_IOCTSIZE) {
		ft_pipe_buffer = arg;
	} else if (cmd == FT_PIPE_IOCQSIZE) {
		return ft_pipe_buffer;
	} else {
		return -ENOTTY;
	}
	return 0;
}

static int	spacefree(struct ft_pipe *dev) {
	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	if (dev->p_read == dev->p_write) {
		return dev->buffersize - 1;
	}
	return ((dev->p_read + dev->buffersize - dev->p_write) % dev->buffersize) - 1;
}

static unsigned int	ft_pipe_poll(struct file *filp, poll_table *wait) {
	struct ft_pipe	*dev = filp->private_data;
	unsigned int	mask = 0;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	mutex_lock(&dev->mutex);
	poll_wait(filp, &dev->inq, wait);
	poll_wait(filp, &dev->outq, wait);
	if (dev->p_read != dev->p_write) {
		mask |= (POLLIN | POLLRDNORM);
	}
	if (spacefree(dev)) {
		mask |= (POLLOUT | POLLWRNORM);
	}
	mutex_unlock(&dev->mutex);
	return mask;
}


static ssize_t ft_pipe_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
	struct ft_pipe	*dev = filp->private_data;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	if (mutex_lock_interruptible(&dev->mutex)) {
		return -ERESTARTSYS;
	}
	while (dev->p_read == dev->p_write) {
		mutex_unlock(&dev->mutex);
		if (filp->f_flags & O_NONBLOCK) {
			return -EAGAIN;
		}
		if (wait_event_interruptible(dev->inq, (dev->p_write != dev->p_read))) {
			return -ERESTARTSYS;
		}
		if (mutex_lock_interruptible(&dev->mutex)) {
			return -ERESTARTSYS;
		}
	}
	if (dev->p_write > dev->p_read) {
		count = min(count, (size_t)(dev->p_write - dev->p_read));
	} else {
		count = min(count, (size_t)(dev->p_end - dev->p_read));
	}
	if (copy_to_user(buf, dev->p_read, count)) {
		mutex_unlock(&dev->mutex);
		return -EFAULT;
	}
	dev->p_read += count;
	if (dev->p_read == dev->p_end) {
		dev->p_read = dev->buffer;
	}
	mutex_unlock(&dev->mutex);
	wake_up_interruptible(&dev->outq);
	return count;
}

static int	ft_getwritespace(struct ft_pipe *dev, struct file *filp) {
	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	while (spacefree(dev) == 0) {
		DEFINE_WAIT(wait);

		mutex_unlock(&dev->mutex);
		if (filp->f_flags & O_NONBLOCK) {
			return -EAGAIN;
		}
		prepare_to_wait(&dev->outq, &wait, TASK_INTERRUPTIBLE);
		if (spacefree(dev) == 0) {
			schedule();
		}
		finish_wait(&dev->outq, &wait);
		if (signal_pending(current)) {
			return -ERESTARTSYS;
		}
		if (mutex_lock_interruptible(&dev->mutex)) {
			return -ERESTARTSYS;
		}
	}
	return 0;
}

static ssize_t	ft_pipe_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
	struct ft_pipe	*dev = filp->private_data;
	int		result;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	if (mutex_lock_interruptible(&dev->mutex)) {
		return -ERESTARTSYS;
	}
	if ((result = ft_getwritespace(dev, filp))) {
		return result;
	}
	count = min(count, (size_t)spacefree(dev));
	if (dev->p_write >= dev->p_read) {
		count = min(count, (size_t)(dev->p_end - dev->p_write));
	} else {
		count = min(count, (size_t)(dev->p_read - dev->p_write - 1));
	}
	if (copy_from_user(dev->p_write, buf, count)) {
		mutex_unlock(&dev->mutex);
		return -EFAULT;
	}
	printk(KERN_ALERT "%s\n", dev->p_write);
	dev->p_write += count;
	if (dev->p_write == dev->p_end) {
		dev->p_write = dev->buffer;
	}
	mutex_unlock(&dev->mutex);
	wake_up_interruptible(&dev->inq);
	if (dev->async_queue) {
		kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
	}
	return count;
}

static int	ft_pipe_fasync(int fd, struct file *filp, int mode) {
	struct ft_pipe	*dev = filp->private_data;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	return fasync_helper(fd, filp, mode, &dev->async_queue);
}

static int	ft_pipe_open(struct inode *inode, struct file *filp) {
	struct ft_pipe	*dev;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	dev = container_of(inode->i_cdev, struct ft_pipe, cdev);
	filp->private_data = dev;

	if (mutex_lock_interruptible(&dev->mutex)) {
		return -ERESTARTSYS;
	}
	if (!dev->buffer) {
		if (!(dev->buffer = kmalloc(sizeof(*dev->buffer) * ft_pipe_buffer, GFP_KERNEL))) {
			mutex_unlock(&dev->mutex);
			return -ENOMEM;
		}
	}
	dev->buffersize = ft_pipe_buffer;
	dev->p_end = dev->buffer + dev->buffersize;
	dev->p_read = dev->p_write = dev->buffer;

	if (filp->f_mode & FMODE_READ) {
		++dev->nreaders;
	}
	if (filp->f_mode & FMODE_WRITE) {
		++dev->nwriters;
	}
	mutex_unlock(&dev->mutex);

	return nonseekable_open(inode, filp);
}

static int	ft_pipe_release(struct inode *inode, struct file *filp) {
	struct ft_pipe	*dev = filp->private_data;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	ft_pipe_fasync(-1, filp, 0);
	mutex_lock(&dev->mutex);
	if (filp->f_mode & FMODE_READ) {
		--dev->nreaders;
	}
	if (filp->f_mode & FMODE_WRITE) {
		--dev->nwriters;
	}
	if (dev->nreaders + dev->nwriters == 0) {
		kfree(dev->buffer);
		dev->buffer = NULL;
	}
	mutex_unlock(&dev->mutex);
	return 0;
}

struct file_operations	ft_pipe_fops = {
	.owner =		THIS_MODULE,
	.read =			ft_pipe_read,
	.write =		ft_pipe_write,
	.unlocked_ioctl =	ft_pipe_ioctl,
	.poll =			ft_pipe_poll,
	.open =			ft_pipe_open,
	.release =		ft_pipe_release,
	.fasync =		ft_pipe_fasync,
} ;

static void __init ft_pipe_setup_cdev(struct ft_pipe *dev, int index) {
	dev_t	devno = ft_pipe_devno + index;

	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);
	cdev_init(&dev->cdev, &ft_pipe_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &ft_pipe_fops;
	if (cdev_add(&dev->cdev, devno, 1)) {
		printk(KERN_WARNING "cdev_add() failed\n");
	}
	printk(KERN_ALERT "End the %s function\n", __FUNCTION__);
}

static int __init ft_pipe_module_init(void) {
	int 	ret_value = 0;
	size_t	i = 0;
	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);

	if (ft_pipe_major) {
		ft_pipe_devno = MKDEV(ft_pipe_major, ft_pipe_minor);
		ret_value = register_chrdev_region(ft_pipe_devno, ft_pipe_nr_devs,"ft_pipe");
	} else {
		ret_value = alloc_chrdev_region(&ft_pipe_devno, 0, ft_pipe_nr_devs, "ft_pipe");
		ft_pipe_major = MAJOR(ft_pipe_devno);
	}
	if (ret_value < 0) {
		goto init_some_chardev_region_failed;
	}
	if (!(ft_pipe_devices = kmalloc(ft_pipe_nr_devs * sizeof(struct ft_pipe), GFP_KERNEL))) {
		goto init_kmalloc_faild;
	}
	memset(ft_pipe_devices, 0, ft_pipe_nr_devs * sizeof(struct ft_pipe));
	while (i < ft_pipe_nr_devs) {
		init_waitqueue_head(&ft_pipe_devices[i].inq);
		init_waitqueue_head(&ft_pipe_devices[i].outq);
		mutex_init(&ft_pipe_devices[i].mutex);
		ft_pipe_setup_cdev(&ft_pipe_devices[i], i);
		++i;
	}
	printk(KERN_ALERT "End the %s function\n", __FUNCTION__);
	return 0;
init_kmalloc_faild :
	unregister_chrdev_region(ft_pipe_devno, ft_pipe_nr_devs);
init_some_chardev_region_failed :
	return ret_value;
}

static void __exit ft_pipe_module_exit(void) {
	size_t	i = 0;
	printk(KERN_ALERT "Start the %s function\n", __FUNCTION__);

	while (i < ft_pipe_nr_devs) {
		cdev_del(&ft_pipe_devices[i].cdev);
		mutex_destroy(&ft_pipe_devices[i].mutex);
		kfree(ft_pipe_devices[i].buffer);
		++i;
	}
	kfree(ft_pipe_devices);
	unregister_chrdev_region(ft_pipe_devno, ft_pipe_nr_devs);
}


module_init(ft_pipe_module_init);
module_exit(ft_pipe_module_exit);
