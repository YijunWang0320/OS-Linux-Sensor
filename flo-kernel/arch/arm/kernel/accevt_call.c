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
static DEV_A head, total, last;
static int totalFrq = 0;
static struct event_unit *event_list;
static int motion_count = 0;
static DECLARE_KFIFO(acc_kfifo,struct dev_acceleration,256);
static int condition = 0;
static int __init initcode()
{
	head.x = 0;
	head.y = 0;
	head.z = 0;
	total.x = 0;
	total.y = 0;
	total.z = 0;
	last.x = 0;
	last.y = 0;
	last.z = 0;
	INIT_KFIFO(acc_kfifo);
	return 0;
}
static void __exit exitcode(void)
{
	return;	
}
int abso(int x)
{
	if(x > 0)
		return x;
	else
		return -x;
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
	new_event->event_count = 0;
	new_event->mBaseline = *tmpAcc;
	new_event->next = NULL;
	init_waitqueue_head(&new_event->mWaitQueue);
	
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
	p->event_count++;
	wait_event_interruptible(p->mWaitQueue, condition == event_id);
	condition = 0;
	return 0;
}
asmlinkage long sys_accevt_signal(struct dev_acceleration __user *acceleration)
{
	int ret;
	struct dev_acceleration *tmpACC;
	struct dev_acceleration  *peek_first;
	struct event_unit *p;
	ACC_M *tmpMotion;

	p = event_list;
	tmpACC = (DEV_A *)kmalloc(sizeof(DEV_A),GFP_KERNEL);
	peek_first = (DEV_A *)kmalloc(sizeof(DEV_A),GFP_KERNEL);
	//tmpMotion = (ACC_M *)kmalloc(sizeof(ACC_M),GFP_KERNEL);
	//before this is just malloc, we need to free this
	//tmplast is the one that before this, static last make it possible
	ret = copy_from_user(tmpACC, acceleration, sizeof(DEV_A));

	if (ret != 0)
	{
		return -1;
	}
	kfifo_put(&acc_kfifo,tmpACC);
	ret = kfifo_len(&acc_kfifo);
	if(abso(tmpACC->x - last.x)+abso(tmpACC->y - last.y)+abso(tmpACC->z - last.z) >= NOISE) {
		total.x += abso(tmpACC->x - last.x);
		total.y += abso(tmpACC->y - last.y);
		total.z += abso(tmpACC->z - last.z);
		totalFrq++;
	}
	if (ret == WINDOW)
	{
		while(p != NULL) {
			if (total.x > p->mBaseline.dlt_x && total.y > p->mBaseline.dlt_y && total.z > p->mBaseline.dlt_z && totalFrq > p->mBaseline.frq) {
				printk("totalFrq: %d,baseline: %d",totalFrq,p->mBaseline.frq);
				condition = p->event_id;
				wake_up_interruptible_all(&p->mWaitQueue);
				}
			p = p->next;
		}
		//tmpMotion->dlt_x = total.x;
		//tmpMotion->dlt_y = total.y;
		//tmpMotion->dlt_z = total.z;
		//tmpMotion->frq = totalFrq;
		kfifo_get(&acc_kfifo,&head);
		kfifo_peek(&acc_kfifo,peek_first);
		if (abso(peek_first->x - head.x) + abso(peek_first->y - head.y) + abso(peek_first->z - head.z) >= NOISE) {
			total.x = total.x - abso(peek_first->x - head.x);
			total.y = total.y - abso(peek_first->y - head.y);
			total.z = total.z - abso(peek_first->z - head.z);
			totalFrq --;
		}
	}
	memcpy(&last, tmpACC, sizeof(DEV_A));
	kfree(tmpACC);
	//kfree(tmpMotion);
	kfree(peek_first);
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
		if (pre == NULL)
			event_list = NULL;
		else
			pre->next = p->next;
		kfree(p);
	}
	return 0;
}
module_init(initcode);
module_exit(exitcode);
