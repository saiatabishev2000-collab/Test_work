#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/init.h>

#define DEVICE_NAME "my_dummy_device"
#define CLASS_NAME "my_dummy_class"

static int major;
static struct class *dummy_class;
static struct cdev dummy_cdev;
static dev_t dev_num;
// Открытие устройства
static int dummy_open(struct inode *inode, struct file *file) {
    pr_info("Device opened\n");
    return 0;
}
// Закрытие устройства
static int dummy_release(struct inode *inode, struct file *file) {
    pr_info("Device closed\n");
    return 0;
}
// Чтение из устройства
static ssize_t dummy_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    pr_info("Read operation\n");
    return 0;
}
// Запись в устройство
static ssize_t dummy_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    pr_info("Write operation (%zu bytes)\n", len);
    return len;
}
// Структура файловых операций
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dummy_open,
    .release = dummy_release,
    .read = dummy_read,
    .write = dummy_write,
};
// Инициализация модуля
static int __init dummy_init(void) {
    // 1. Выделяем номера устройств
    if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0) {
        pr_err("Failed to allocate device numbers\n");
        return -1;
    }
    major = MAJOR(dev_num);
    
    // 2. Инициализируем cdev
    cdev_init(&dummy_cdev, &fops);
    dummy_cdev.owner = THIS_MODULE;
    
    // 3. Добавляем cdev в систему
    if (cdev_add(&dummy_cdev, dev_num, 1) < 0) {
        unregister_chrdev_region(dev_num, 1);
        pr_err("Failed to add cdev\n");
        return -1;
    }
    
    // 4. Создаем класс устройств
    dummy_class = class_create(CLASS_NAME);
    if (IS_ERR(dummy_class)) {
        cdev_del(&dummy_cdev);
        unregister_chrdev_region(dev_num, 1);
        pr_err("Failed to create class\n");
        return PTR_ERR(dummy_class);
    }
    
    // 5. Создаем устройство в /dev
    if (IS_ERR(device_create(dummy_class, NULL, dev_num, NULL, DEVICE_NAME))) {
        class_destroy(dummy_class);
        cdev_del(&dummy_cdev);
        unregister_chrdev_region(dev_num, 1);
        pr_err("Failed to create device\n");
        return -1;
    }
    
    pr_info("Dummy driver loaded (major=%d)\n", major);
    return 0;
}
// Выгрузка модуля
static void __exit dummy_exit(void) {
    device_destroy(dummy_class, dev_num);
    class_destroy(dummy_class);
    cdev_del(&dummy_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("Dummy driver unloaded\n");
}

module_init(dummy_init);
module_exit(dummy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("vboxuser");
MODULE_DESCRIPTION("Fixed dummy driver for kernel 6.14+");
