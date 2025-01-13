#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "custom_char_device"
#define IOCTL_CLEAR_BUFFER _IO('b', 1)
#define IOCTL_RESIZE_BUFFER _IOW('b', 2, int)

static int major_number;
static char *device_buffer;
static size_t buffer_capacity = 1024;

// Чтение из устройства
static ssize_t device_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset) {
    if (*offset >= buffer_capacity) {
        return 0; // Конец файла
    }

    if (*offset + count > buffer_capacity) {
        count = buffer_capacity - *offset; // Ограничиваем количество байт
    }

    if (copy_to_user(user_buffer, device_buffer + *offset, count)) {
        return -EFAULT; // Ошибка копирования
    }

    *offset += count;
    return count;
}

// Запись в устройство
static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset) {
    if (*offset >= buffer_capacity) {
        return -ENOSPC; // Нет места для записи
    }

    if (*offset + count > buffer_capacity) {
        count = buffer_capacity - *offset; // Ограничиваем запись
    }

    if (copy_from_user(device_buffer + *offset, user_buffer, count)) {
        return -EFAULT; // Ошибка копирования
    }

    *offset += count;
    return count;
}

// Управление устройством через IOCTL
static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case IOCTL_CLEAR_BUFFER:
            memset(device_buffer, 0, buffer_capacity); // Очистка буфера
            break;
        case IOCTL_RESIZE_BUFFER:
            if (arg > 0) {
                char *new_buffer = krealloc(device_buffer, arg, GFP_KERNEL);
                if (!new_buffer) {
                    return -ENOMEM; // Ошибка выделения памяти
                }
                device_buffer = new_buffer;
                buffer_capacity = arg;
                memset(device_buffer, 0, buffer_capacity); // Инициализация нового буфера
            }
            break;
        default:
            return -EINVAL; // Неверная команда
    }
    return 0;
}

// Операции с файлом
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl,
};

// Инициализация модуля
static int __init custom_char_device_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Не удалось зарегистрировать символьное устройство\n");
        return major_number;
    }

    device_buffer = kmalloc(buffer_capacity, GFP_KERNEL);
    if (!device_buffer) {
        unregister_chrdev(major_number, DEVICE_NAME);
        return -ENOMEM;
    }

    memset(device_buffer, 0, buffer_capacity); // Инициализация буфера
    printk(KERN_INFO "Символьное устройство зарегистрировано с номером %d\n", major_number);
    return 0;
}

// Завершение работы модуля
static void __exit custom_char_device_exit(void) {
    kfree(device_buffer);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Символьное устройство удалено\n");
}

module_init(custom_char_device_init);
module_exit(custom_char_device_exit);
MODULE_LICENSE("GPL");
