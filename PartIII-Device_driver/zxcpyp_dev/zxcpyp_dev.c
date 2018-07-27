/*
 * HUST OS Design - Part III
 * 
 * Device driver - Character device driver
 * 
 * Created by zxcpyp at 2018-07-22
 * 
 * Github: zxc479773533
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define ZXCPYPMEM_SIZE 0x4000  /* 8KB for zxcpyp driver */
#define GLOBAL_MAJOR 666  /* Set device major */
#define MEM_CLEAR 1  /* Use for ioctl to clear memory */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zxcpyp <zxc479773533@gmail.com>");
MODULE_DESCRIPTION("A simple character driver module");

struct zxcpyp_dev {
  struct cdev cdev;
  unsigned char mem[ZXCPYPMEM_SIZE];
};

static struct zxcpyp_dev *zxcpyp_devp;  /* Global dev pointer */
struct device *device;
struct class *class;

/*
 * zxcpypdriver_open - Open the driver
 */
static int zxcpypdriver_open(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "ZXCPYP Driver: open\n");
  filep->private_data = zxcpyp_devp;
  return 0;
}

/*
 * zxcpypdriver_release - Release the driver
 */
static int zxcpypdriver_release(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "ZXCPYP Driver: release\n");
  return 0;
}

/*
 * zxcpypdriver_read - Read from the driver
 */
static ssize_t zxcpypdriver_read(struct file *filep, char __user *buf, size_t count, loff_t *offset) {
  printk(KERN_INFO "ZXCPYP Driver: start read\n");

  int ret = 0;
  size_t avail = ZXCPYPMEM_SIZE - *offset;
  struct zxcpyp_dev *dev = filep->private_data;

  /* Available memory exists */
  if (count <= avail) {
    if (copy_to_user(buf, dev->mem + *offset, count) != 0)
      return -EFAULT;
    *offset += count;
    ret = count;
  }
  /* Available memory not enough */
  else {
    if (copy_to_user(buf, dev->mem + *offset, avail) != 0)
      return -EFAULT;
    *offset += count;
    ret = avail;
  }

  printk(KERN_INFO "ZXCPYP Driver: read %u bytes\n", ret);
  return ret;
}

/*
 * zxcpypdriver_write - Write from the driver
 */
static ssize_t zxcpypdriver_write(struct file *filep, const char __user *buf, size_t count, loff_t *offset) {
  printk(KERN_INFO "ZXCPYP Driver: start write\n");

  int ret = 0;
  size_t avail = ZXCPYPMEM_SIZE - *offset;
  struct zxcpyp_dev *dev = filep->private_data;
  memset(dev->mem + *offset, 0, avail);

  /* Available memory exists */
  if (count <= avail) {
    if (copy_from_user(buf, dev->mem + *offset, count) != 0)
      return -EFAULT;
    *offset += count;
    ret = count;
  }
  /* Available memory not enough */
  else {
    if (copy_from_user(buf, dev->mem + *offset, avail) != 0)
      return -EFAULT;
    *offset += count;
    ret = avail;
  }

  printk(KERN_INFO "ZXCPYP Driver: written %u bytes\n", ret);
  return ret;
}

/*
 * zxcpypdriver_llseek - Set the current position of the file for reading and writing
 */
static loff_t zxcpypdriver_llseek(struct file *filep, loff_t offset, int whence) {
  printk(KERN_INFO "ZXCPYP Driver: start llseek\n");

  loff_t ret = 0;
  switch (whence) {
  /* SEEK_SET */
  case 0:
    if (offset < 0) {
      ret = -EINVAL;
      break;
    }
    if (offset > ZXCPYPMEM_SIZE) {
      ret = -EINVAL;
      break;
    }
    ret = offset;
    break;
  /* SEEK_CUR*/
  case 1:
    if ((filep->f_pos + offset) > ZXCPYPMEM_SIZE) {
      ret = -EINVAL;
      break;
    }
    if ((filep->f_pos + offset) < 0) {
      ret = -EINVAL;
      break;
    }
    ret = filep->f_pos + offset;
    break;
  /* 
   * SEEK_END: Here we can't use SEEK_END,
   *           beacuse the memory is solid.
   *
  case 2:
    if (offset < 0) {
      ret = -EINVAL;
      break;
    }
    ret = ZXCPYPMEM_SIZE + offset;
    break;*/
  /* Else: return error */
  default:
    ret = -EINVAL;
  }

  if (ret < 0)
    return ret;

  printk(KERN_INFO "ZXCPYP Driver: set offset to %u\n", ret);
  filep->f_pos = ret;
  return ret;
}

/*
 * zxcpypdriver_ioctl - Control the zxcpyp driver(memory clear)
 */
static long zxcpypdriver_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) {
  printk(KERN_INFO "ZXCPYP Driver: start memory clear\n");

  switch (cmd) {
  case MEM_CLEAR:
    memset(filep->private_data, 0, ZXCPYPMEM_SIZE);
    printk("ZXCPYP Driver: memory is set to zero\n");
    break;
  default:
    return -EINVAL;
  }
  return 0;
}

/*
 * Set operation pointers
 */
static const struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = zxcpypdriver_open,
  .release = zxcpypdriver_release,
  .read = zxcpypdriver_read,
  .write = zxcpypdriver_write,
  .llseek = zxcpypdriver_llseek,
  .unlocked_ioctl = zxcpypdriver_ioctl,
};

/*
 * zxcpypdriver_init - Initial function for zxcpypdriver
 */
static int __init zxcpypdriver_init(void) {
  printk(KERN_INFO "Load module: zxcpypdriver\n");

  int ret;
  dev_t devno = MKDEV(GLOBAL_MAJOR, 0);
  ret = register_chrdev_region(devno, 1, "zxcpypdriver");
  if (ret < 0) {
    printk(KERN_ALERT "Registering the character device failed with %d\n", ret);
    return ret;
  }

  /* Alloc memory for device */
  zxcpyp_devp = kzalloc(sizeof(struct zxcpyp_dev), GFP_KERNEL);
  if (zxcpyp_devp == NULL) {
    printk(KERN_ALERT "Alloc memory for device failed\n");
    ret = -ENOMEM;
    goto failed;
  }

  /* Setup device */
  cdev_init(&zxcpyp_devp->cdev, &fops);
  zxcpyp_devp->cdev.owner = THIS_MODULE;
  cdev_add(&zxcpyp_devp->cdev, devno, 1);

  /* Creat device file */
  class = class_create(THIS_MODULE, "zxcpypdriver");
  if (IS_ERR(class)) {
    ret = PTR_ERR(class);
    printk(KERN_ALERT "Creat class for device file failed with %d\n", ret);
    goto failed;
  }
  device = device_create(class, NULL, devno, NULL, "zxcpypdriver");
  if (IS_ERR(device)) {
    class_destroy(class);
    ret = PTR_ERR(device);
    printk(KERN_ALERT "Creat device file failed with %d\n", ret);
    goto failed;
  }

  return 0;

 failed:
  unregister_chrdev_region(devno, 1);
  return ret;
}

/*
 * zxcpypdriver_exit - Exit function for zxcpypdriver
 */
static void __exit zxcpypdriver_exit(void) {
  printk(KERN_INFO "Load module: zxcpypdriver\n");

  device_destroy(class, MKDEV(GLOBAL_MAJOR, 0));
  class_unregister(class);
  class_destroy(class);

  cdev_del(&zxcpyp_devp->cdev);
  kfree(zxcpyp_devp);
  unregister_chrdev_region(MKDEV(GLOBAL_MAJOR, 0), 1);
}

module_init(zxcpypdriver_init);
module_exit(zxcpypdriver_exit);
