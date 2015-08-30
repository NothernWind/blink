#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kaljan");
MODULE_DESCRIPTION("LED Driver. LED connected to GPIO26");

static int led_gpio_pin = 26;
static int led_status = 0;
static int blink_time = 1000;

static struct gpio led;
static struct timer_list blink_timer; 

module_param(led_gpio_pin, int, 0755);
module_param(blink_time, int, 0755);

static void blink_timer_callback(unsigned long data)
{
	printk(KERN_NOTICE "timer_callback called (%ld): %lu.\n", jiffies, data);
	led_status = (led_status) ? 0 : 1;
	gpio_set_value(led_gpio_pin, led_status);
	mod_timer(&blink_timer, jiffies + msecs_to_jiffies(blink_time));
}

static int __init GPIO26_init(void)
{
	int r;
	printk(KERN_NOTICE "gpio_init\n");
	led.gpio = led_gpio_pin;
	led.flags = GPIOF_OUT_INIT_LOW;
	led.label = "Blue LED";
	
	if (!gpio_is_valid(led_gpio_pin)) {
		printk(KERN_ALERT " GPIO number %d not valid.\n", led_gpio_pin);
		return -1;
	}
	
	r = gpio_request(led_gpio_pin,"Blue LED");
	
	if (r) {
		printk(KERN_ALERT "Gpio request failed.\n");
		return -2;
    }
	
	printk(KERN_NOTICE "Set GPIO26 direction output\n");
	gpio_direction_output(led_gpio_pin, 0);
	printk(KERN_NOTICE "LED On\n");
	gpio_set_value(led_gpio_pin, led_status);
	
	setup_timer(&blink_timer, blink_timer_callback, 10);
	
	r = mod_timer(&blink_timer, jiffies + msecs_to_jiffies(blink_time));
	if (r) printk(KERN_ALERT "Error in mod_timer\n");
	
	return 0;
}

static void __exit GPIO26_exit(void)
{
	printk(KERN_NOTICE "LED Off\n");
	gpio_set_value(led_gpio_pin, 0);
	gpio_free(led.gpio);
	printk(KERN_NOTICE "ldrv module exit\n");
}

module_init(GPIO26_init);
module_exit(GPIO26_exit);
