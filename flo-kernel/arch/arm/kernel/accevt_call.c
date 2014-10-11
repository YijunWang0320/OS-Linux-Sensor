#include<linux/syscalls.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include<linux/kfifo.h>
#include<linux/wait.h>

#define ACC_M struct acc_motion
#define DEV_A struct dev_acceleration
struct event_unit {
	ACC_M mBaseline;
	wait_queue_head_t mWaitQueue;
	int event_id;
	int event_count; 
	struct event_unit *next;
};
static struct event_unit *event_list;
static int motion_count = 0;
static DECLARE_KFIFO(acc_kfifo,struct dev_acceleration,256);
static int condition = 0;
static int __init initcode()
{
	INIT_KFIFO(acc_kfifo);
	return 0;
}
static void __exit exitcode(void)
{
	return;	
}
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
	motion_count++;
	struct event_unit *new_event;
	new_event = (struct event_unit *)kmalloc(sizeof(struct event_unit),GFP_KERNEL);
	new_event->event_id = motion_count;
	new_event->event_count = 1;
	new_event->mBaseline = *tmpAcc;
	new_event->next = NULL;
	
	if (event_list==NULL) {
		event_list=new_event;
	} else {
		new_event->next = event_list;
		event_list = new_event;	
	}
	if (motion_count==10) {
		struct event_unit * p = event_list;
		while(p!= NULL) {
			printk("event: eventid=%d",p->event_id);
			p = p->next;
		}
	}
	
	kfree(tmpAcc);
	return motion_count;
}
asmlinkage long sys_accevt_wait(int event_id)
{
	struct event_unit *p;

	p = event_list;
	while (p != NULL) {
		if (p->event_id == event_id)
			break;
		else
			p = p->next;
	}
	if (p == NULL)
		return -1;
	wait_event_interruptible(p->mWaitQueue, condition == event_id);
	return 0;
}
asmlinkage long sys_accevt_signal(struct dev_acceleration __user *acceleration)
{
	int ret;
	int i = 0;
	struct dev_acceleration *tmpACC, *latACC;
	int formerX, formerY, formerZ, laterX, laterY, laterZ, frq;
	struct event_unit *p;

	p = event_list;
	tmpACC = (DEV_A *)kmalloc(sizeof(DEV_A),GFP_KERNEL);
	latACC = (DEV_A *)kmalloc(sizeof(DEV_A),GFP_KERNEL);
	ret = copy_from_user(tmpACC, acceleration, sizeof(DEV_A));
	if (ret != 0)
	{
		return -1;
	}
	kfifo_put(&acc_kfifo,tmpACC);
	ret = kfifo_len(&acc_kfifo);
	if(ret == WINDOW)
		while(!p = NULL) {
			frq = 0;
			formerX = 0;
			formerY = 0;
			formerZ = 0;
			for(i = 0 ;i<WINDOW ;i++) {
				kfifo_get(&acc_kfifo,latACC);
				laterX = latACC->x;
				laterY = latACC->y;
				laterZ = latACC->z;
				if(laterX - formerX > p->mBaseline.dlt_x && laterY-formerY > p->mBaseline.dlt_y && laterZ - formerZ > p->mBaseline.dlt_z) {
					frq++;
				}
			}
			if (frq >= p->mBaseline.frq) {
				wake_up_interruptible(&p->mWaitQueue);
			}
		}
	return ret;
}
asmlinkage long sys_accevt_destroy(int event_id)
{
	struct event_unit *p, *pre;

	p = event_list;
	pre = NULL;
	while (p != NULL) {
		if (p->event_id == event_id)
			break;
		else {
			pre = p;
			p = p->next;
		}
	}
	if (p == NULL) 
		return -1;
	if (p->event_count > 1)
		p->event_count--;
	else {
		pre->next = p->next;
		kfree(p);
	}
	struct event_unit * ptr = event_list;
	while(ptr!= NULL) {
		printk("In destroy: eventid = %d \n",ptr->event_id);
		ptr = ptr->next;
	}
	return 0;
}
module_init(initcode);
module_exit(exitcode);
