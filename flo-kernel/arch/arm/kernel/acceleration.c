#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
asmlinkage long sys_set_acceleration(struct dev_acceleration __user *acceleration){
	
	if(acceleration == NULL)
		return -1;
	struct dev_acceleration *accelerationInfo = (struct dev_acceleration*)kmalloc(sizeof(struct dev_acceleration),GFP_KERNEL);
	int i;
	i = copy_from_user(accelerationInfo, acceleration, sizeof(struct dev_acceleration));
	if(i != 0)
		return -1;
	printk("acceleration.x = %d \n", accelerationInfo->x);
	printk("acceleration.y = %d \n", accelerationInfo->y);
	printk("acceleration.z = %d \n", accelerationInfo->z);
	kfree(accelerationInfo);
	return 10;
}
