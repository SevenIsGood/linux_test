#include <linux/module.h>

#include <linux/slab.h>
#include <linux/cdev.h>

#include <linux/init.h>

#include <linux/fs.h>
#include <linux/uaccess.h>

#define GLOBALMEM_SIZE 1024
#define GLOBALMEM_MAJOR 230

static int globalmem_major = GLOBALMEM_MAJOR;
module_param(globalmem_major, int, S_IRUGO);

struct globalmem_dev {
    struct cdev c_dev;
    unsigned char mem[GLOBALMEM_SIZE];
};

struct globalmem_dev *globalmem_devp;

static int globalmem_open(struct inode *inode, struct file *filp) {
    filp->private_data = globalmem_devp;
    return 0;
}

static int globalmem_release(struct inode *inode, struct file *filp) {
    return 0;
}

static ssize_t globalmem_write(struct file *filp, const char __user *buf,
    size_t size, loff_t *ppos) 
{
    // offset get from *ppos
    unsigned long p = *ppos;
    struct globalmem_dev *dev = filp->private_data;
    ssize_t ret = 0;

    unsigned long count = size;

    if (p >= GLOBALMEM_SIZE) {
        return 0;
    }

    if (count + p > GLOBALMEM_SIZE)
        count = GLOBALMEM_SIZE - p;

    //check copy result
    if (copy_from_user(dev->mem + p, buf, count)) {
        ret = -EFAULT;
    }
    else {

        // update ppos, ther kernel will renew this to filp properly
        *ppos += count;
        ret = count;

        printk(KERN_INFO "write %lu bytes from %lu\n", ret, p);
    }

    return ret; 
}

static ssize_t globalmem_read(struct file *filp, char __user *buf,
size_t size, loff_t *offp) 
{
    struct globalmem_dev *dev = filp->private_data;
    unsigned long p = *offp;
    unsigned long count = size;
    ssize_t ret = 0;

    if (p >= GLOBALMEM_SIZE) {
        return 0;
    }

    if (count >= GLOBALMEM_SIZE - p) {
        count = GLOBALMEM_SIZE - p;
    }

    if (copy_to_user(buf, dev->mem + p, count)) {
        ret = -EFAULT;
    }
    else {
        ret = count;
        *offp += count;
        printk(KERN_NOTICE "read %lu byte from %lu\n", count, p);
    }

    return ret;
}

static loff_t globalmem_llseek(struct file *filp, loff_t offset, int orig) {
    struct globalmem_dev *dev = filp->private_data;

    loff_t ret = 0;

    switch(orig) {
        case 0:
        ret = offset;
        break;
        
        case 1:
        ret = filp->f_pos + offset;
        break;

        case 2:
        ret = GLOBALMEM_SIZE + offset;
        break;

        default:
        ret = -EFAULT;
    }

    if (ret < 0) 
        ret = -EFAULT;

    filp->f_pos = ret;

    return ret;
}

static long globalmem_ioctl(struct file *filp,
    unsigned int cmd, unsigned long arg)
{
    struct globalmem_dev *dev = filp->private_data;

    switch (cmd) {
        case 1:
            memset(dev->mem, 0 , GLOBALMEM_SIZE);
            printk(KERN_INFO "globalmem is set to zero\n");
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

static const struct file_operations globalmem_ops = 
{
    .owner = THIS_MODULE,
    .open = globalmem_open,
    .release = globalmem_release,
    .llseek = globalmem_llseek,
    .read = globalmem_read,
    .write = globalmem_write,
    .unlocked_ioctl = globalmem_ioctl,
};

static void globalmem_setup(struct globalmem_dev *dev, 
    int index) {
    dev_t devno = MKDEV(globalmem_major, index);
    int err;

    cdev_init(&dev->c_dev, &globalmem_ops);
    dev->c_dev.owner = THIS_MODULE;

    err = cdev_add(&dev->c_dev, devno, 1);
    if (err) {
        printk(KERN_NOTICE "Error %d adding gloablmem%d", err, index);
    }
}

static int __init globalmem_init (void) {
    int ret;

    dev_t dev_no = MKDEV(globalmem_major, 0);

    if (globalmem_major) {
        ret = register_chrdev_region(dev_no, 1, "globalmem");
    }
    else {
        ret = alloc_chrdev_region(&dev_no, 0, 1, "globalmem");
        globalmem_major = MAJOR(dev_no);
    }

    globalmem_devp = 
         kzalloc(sizeof(struct globalmem_dev), GFP_KERNEL);

    if (!globalmem_devp) {
        ret = -ENOMEM;
        goto fail;
    }

    globalmem_setup(globalmem_devp, 0);
    return 0;

fail:
    unregister_chrdev_region(dev_no, 1);
    return ret;
}
module_init(globalmem_init);

static void __exit globalmem_exit (void) {
    cdev_del(&globalmem_devp->c_dev);
    kfree(globalmem_devp);
    unregister_chrdev_region(MKDEV(globalmem_major, 0), 1);
}
module_exit(globalmem_exit);

MODULE_AUTHOR("tang");
MODULE_LICENSE("GPL v2");
