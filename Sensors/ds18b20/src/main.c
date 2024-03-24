/*
 * Copyright (c) 2022 Thomas Stranger
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <string.h>


/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

/* delay between greetings (in ms) */
#define SLEEPTIME 500

//The stack area must be defined using K_THREAD_STACK_DEFINE or K_KERNEL_STACK_DEFINE to ensure it is properly set up in memory.
K_THREAD_STACK_DEFINE(threadA_stack_area, STACKSIZE);
K_THREAD_STACK_DEFINE(threadB_stack_area, STACKSIZE);


static struct k_thread threadA_data,threadB_data;



/*
 * Get a device structure from a devicetree node with compatible
 * "maxim,ds18b20". (If there are multiple, just pick one.)
 */
static const struct device *get_ds18b20_device(void)
{
	const struct device *const dev = DEVICE_DT_GET_ANY(maxim_ds18b20);

	if (dev == NULL) {
		/* No such node, or the node does not have status "okay". */
		printk("\nError: no device found.\n");
		return NULL;
	}

	if (!device_is_ready(dev)) {
		printk("\nError: Device \"%s\" is not ready; "
		       "check the driver initialization logs for errors.\n",
		       dev->name);
		return NULL;
	}

	printk("Found device \"%s\", getting sensor data\n", dev->name);
	return dev;
}


void threadA(void *dev, void *dummy2, void *dummy3)
{
  //ARG_UNUSED(dummy1);
  	ARG_UNUSED(dummy2);
  	ARG_UNUSED(dummy3);

	const struct device *device=(const struct device *)dev;

	printk("thread_a: thread started \n");
	struct sensor_value temp;

	while (1)
	{
	  sensor_sample_fetch(device);
	  sensor_channel_get(device, SENSOR_CHAN_AMBIENT_TEMP, &temp);
	  
	  printk("Temp: %d.%06d\n", temp.val1, temp.val2);
	  k_sleep(K_MSEC(10000));	  	
	

	  printk("thread_a: thread loop \n");
	  //k_msleep(SLEEPTIME);
	}

}


int main(void)
{
	const struct device *dev = get_ds18b20_device();

	if (dev == NULL) {
		return 0;
	}

	k_tid_t my_tid1= k_thread_create(&threadA_data, threadA_stack_area,
			K_THREAD_STACK_SIZEOF(threadA_stack_area),
					 threadA,(void *) dev, NULL, NULL,
			PRIORITY, 0, K_FOREVER);
	k_thread_name_set(&threadA_data, "thread_a");

	k_thread_start(&threadA_data);

	
	/*
	while (1) {
		struct sensor_value temp;

		sensor_sample_fetch(dev);
		sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);

		printk("Temp: %d.%06d\n", temp.val1, temp.val2);
		k_sleep(K_MSEC(10000));
	}
	*/
	return 0;
}
