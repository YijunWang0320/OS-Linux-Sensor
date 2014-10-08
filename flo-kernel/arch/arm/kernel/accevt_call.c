#include<linux/syscalls.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include<linux/kfifo.h>
#define ACC_M struct acc_motion
static ACC_M motionGroup[10];
static int motion_count = 0;

asmlinkage long sys_accevt_create(struct acc_motion __user *acceleration)
{
	ACC_M *tmpAcc;
	int ret = 0;

	if(acceleration == NULL)
		return -1;
	tmpAcc = (ACC_M *)kmalloc(sizeof(ACC_M), GFP_KERNEL);
	ret = copy_from_user(tmpAcc, acceleration, sizeof(ACC_M));
	if (ret != 0)
		return -1;
	motion[mount_count] = *tmpAcc;
	motion_count++;
	kfree(tmpAcc);
	return motion_count-1;
}
asmlinkage long sys_accevt_wait(int event_id)
{
	return 0;
}
asmlinkage long sys_accevt_signal(struct dev_acceleration __user *acceleration)
{
	return 0;
}
asmlinkage long sys_accevt_destroy(int event_id)
{
	return 0;
}
