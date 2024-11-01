#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>


MODULE_AUTHOR("IO-24 Slobodeniuk Sasha");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static uint counter = 1;
module_param(counter, uint, 0444);
MODULE_PARM_DESC(counter, "Number of times to print 'Hello, world!' message");

struct hello_data {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(hello_list);

static int __init hello_init(void)
{
	struct hello_data *data;
	int i;

	if (counter == 0 || (counter >= 5 && counter <= 10)) {
		printk(KERN_WARNING "Warning: counter parameter is 0 or between 5 and 10. Continuing.\n");
	} else if (counter > 10) {
		printk(KERN_ERR "Error: counter parameter is greater than 10. Module not loaded.\n");
		return -EINVAL;
	}

	for (i = 0; i < counter; i++) {
		data = kmalloc(sizeof(*data), GFP_KERNEL);
		if (!data) {
			printk(KERN_ERR "Failed to allocate memory\n");
			return -ENOMEM;
		}

		data->time = ktime_get();
		list_add_tail(&data->list, &hello_list);

		printk(KERN_EMERG "Hello, world!\n");
	}
	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_data *data, *tmp;

	list_for_each_entry_safe(data, tmp, &hello_list, list) {
		printk(KERN_INFO "Time: %lld ns\n", ktime_to_ns(data->time));
		list_del(&data->list);
		kfree(data);
	}
}

module_init(hello_init);
module_exit(hello_exit);
