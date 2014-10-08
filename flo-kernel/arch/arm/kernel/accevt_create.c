#include<linux/syscalls.h>
#include<linux/uaccess.h>
#incdlue<linux/slab.h>
#define ACC_M struct acc_motion
asmlinkage long sys_accevt_create(struct acc_motion __user *acceleration)
{
	extern struct acc_motion group[20];
	extern int motion_count;
	ACC_M *tmpAcc;
	int ret = 0;
	int i=0;

	if(acceleration == null)
		return -1;
	tmpAcc = (ACC_M *)kmalloc(sizeof(ACC_M), GFP_KERNEL);
	ret = copy_to_user(tmpAcc, acceleration, sizeof(ACC_M));
	if (ret != 0)
		return -1;
	group[motion_count] = *tmpAcc;
	motion_count++;
	if(motion_count == 10){
		for(i = 0; i<10; i++){
			printk("motion count%d:%d\n",i,group[i].frq);
		}
	}
	return 0;
}

