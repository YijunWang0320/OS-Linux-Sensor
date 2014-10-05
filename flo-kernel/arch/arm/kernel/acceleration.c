#include <linux/syscalls.h>
asmlinkage long sys_set_acceleration(struct dev_acceleration __user *acceleration){
	return 10;
}
