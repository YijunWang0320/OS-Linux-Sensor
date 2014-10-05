#include <linux/syscalls.h>
#include <linux/uaccess.h>
asmlinkage long sys_set_acceleration(struct dev_acceleration __user *acceleration){
	
	struct dev_acceleration *accelerationInfo = acceleration;
	int i;
	i = copy_from_user(&accelerationInfo, acceleration, sizeof(dev_acceleration));
	if(i != 0)
		return -1;
	printk("acceleration.x = %d \n", accelerationInfo->x);
	printk("acceleration.y = %d \n", accelerationInfo->y);
	printk("acceleration.z = %d \n", accelerationInfo->z);

	return 10;
}
