#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/kernel.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>


/* From hello.c */
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#include <linux/timekeeping.h>


#define DEVICE_NAME "meschar"
#define CLASS_NAME "mes"

static DEFINE_MUTEX(meschar_mutex);

static int majorNumber;
static struct class* mescharClass = NULL;
static struct device* mescharDevice = NULL;
static char *test_str = "Hello there";
// static char long_test_str[4096];

static ulong rot_times[3];
static ulong last_time, new_time, rot_time;

static int device_open(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
// static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);
static int device_release(struct inode *, struct file *);

// long speed;
// module_param(speed,long,S_IRUGO);
module_param(rot_time,ulong,S_IRUGO);
// module_param(last_time,long,S_IRUGO);

static struct file_operations fops = 
{
    .open = device_open,
    .read = device_read,
    // .write = device_write,
    .release = device_release,
};

/* End from hello.c*/


// Variable declaration
struct gpio_desc *gpio_button;
unsigned int irq_number;
long error_count;

// Interrupt service routine
static irq_handler_t irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
    printk("Interrupt Has Occurred\n");
    new_time = ktime_get_ns() / 10000;
    // Check if it's been longer than 1 ms since the last rotation
    if (new_time - last_time > 1) {
        rot_times[2] = rot_times[1];
        rot_times[1] = rot_times[0];
        rot_times[0] = new_time - last_time;
        rot_time = (rot_times[0] + rot_times[1] + rot_times[2]) / 3;
        last_time = new_time;
    }
    return (irq_handler_t) IRQ_HANDLED; 
}

// Probe function
static int led_probe(struct platform_device *pdev) // edited
{
    printk("LED Probe start\n");
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    mescharClass = class_create(THIS_MODULE, CLASS_NAME);
    mescharDevice = device_create(mescharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    mutex_init(&meschar_mutex);

    
    // Gets GPIOs for button and LED
    printk("Getting gpiod...\n");
    gpio_button = devm_gpiod_get(&pdev->dev, "userbutton", GPIOD_IN);
    printk("Gpiod to irq...\n");
    // gpio_led = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
    irq_number = gpiod_to_irq(gpio_button); // descriptor instead of index
    printk("irq_number: %u\n", irq_number);
    printk("Requesting irq...\n");
    // Requests IRQ
    if(request_irq(irq_number, (irq_handler_t) irq_handler, IRQF_TRIGGER_RISING, "userbutton", pdev) != 0) { // null?
        printk("asdsadasd \n");
        return -1;
    };
    // Debounce function set for 1000000 ms
    gpiod_set_debounce(gpio_button, 1000000);

    printk("LED Probe finished\n");
    // printk(KERN_INFO "Oh hi mark - I love Lisa X more than you do\n");

    return 0;
}

// Remove function
static int led_remove(struct platform_device *pdev)
{
    printk("Starting remove...\n");
    printk("irq_number: %u\n", irq_number);
    free_irq(irq_number, pdev);
    printk("LED removed \n");


    device_destroy(mescharClass, MKDEV(majorNumber,0));
    class_unregister(mescharClass);
    class_destroy(mescharClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    mutex_destroy(&meschar_mutex);
    // printk(KERN_INFO "sad, but still love Lisa X more than you\n");
    printk("LED Remove finished\n");

    return 0;
}

/* From hello.c */

// static int __init hello_init(void){
//     majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
//     mescharClass = class_create(THIS_MODULE, CLASS_NAME);
//     mescharDevice = device_create(mescharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
//     printk(KERN_INFO "Oh hi mark - I love Lisa X more than you do\n");
//     mutex_init(&meschar_mutex);

//     return 0;
// }

// static void __exit hello_exit(void){
//     device_destroy(mescharClass, MKDEV(majorNumber,0));
//     class_unregister(mescharClass);
//     class_destroy(mescharClass);
//     unregister_chrdev(majorNumber, DEVICE_NAME);
//     mutex_destroy(&meschar_mutex);
//     printk(KERN_INFO "sad, but still love Lisa X more than you\n");
// }

static int device_open(struct inode *inodep, struct file *filep){
    printk("Opening device...\n");
    if(!mutex_trylock(&meschar_mutex)){
        printk(KERN_ALERT "I'm being used!\n");
        return -EBUSY;
    }
    printk("Device opened\n");
    // printk(KERN_INFO "You're tearing me apart, Lisa! Also I've been opened some times.\n");
    return 0;
}

static ssize_t device_read(struct file *filep, char __user *buf, size_t length, loff_t *offset){
    printk("Reading device...\n");

    rot_time = 1000000;

    // long error_count;
    // error_count = copy_to_user(buf, &rot_time, sizeof(rot_time));
    // memset(long_test_str, 0, 4096);
    // memset(long_test_str, 80, 4095);
    error_count = copy_to_user(buf, test_str, strlen(test_str));
    // error_count = copy_to_user(buf, test_str, strlen(test_str));
    // printk("Strlen: %lu\n", strlen(test_str));

    // printk("Buffer: %s", buf);

    // printk("Error count: %ld\n", error_count);
    // printk("User buf length %lu\n", length);
    // printk("Sent %ld characters back\n", sizeof(rot_time));
    // printk("Rotation time: %ld\n", rot_time);
    return 0;
}

static int device_release(struct inode *inodep, struct file *filep) {
    printk("Releasing device...\n");
    mutex_unlock(&meschar_mutex);
    //being_used--;

    // printk("I'll never let go, Jack. I'll never let go. I promise.\n");
    printk("Device released\n");
    return 0;
}

/* End from hello.c*/

// Compatibility with device tree
static struct of_device_id matchy_match[] = {
    {
        .compatible = "gpiod_driver",
    },
    {/* leave alone - keep this here (end node) */},
};

// Platform driver object
static struct platform_driver adam_driver = {
    .probe	 = led_probe,
    .remove	 = led_remove, 
    .driver	 = {
           .name  = "The Rock: this name doesn't even matter",
           .owner = THIS_MODULE,
           .of_match_table = matchy_match, // edited
    },
};

module_platform_driver(adam_driver); // edited

MODULE_DESCRIPTION("424\'s finest");
MODULE_AUTHOR("GOAT");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:adam_driver");

// module_init(hello_init);
// module_exit(hello_exit);
