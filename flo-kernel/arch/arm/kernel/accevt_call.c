#include<linux/syscalls.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include<linux/kfifo.h>
#include<linux/wait.h>
#include<linux/spinlock.h>
# include <error.h>
#define ACC_M struct acc_motion
#define DEV_A struct dev_acceleration
#define E_U struct event_unit
struct event_unit {
	ACC_M mBaseline;
	wait_queue_head_t mWaitQueue;
	int event_id;
	int event_count;
	struct event_unit *next;
};
static DEV_A head, total, last;/*only signal */
static int totalFrq;/*only signal*/
static struct event_unit *event_list;/* lock 1*/
static int motion_count;/*lock 2*/
static DECLARE_KFIFO(acc_kfifo, struct dev_acceleration, 256);
static int condition;
static spinlock_t motion_count_lock;
static spinlock_t event_list_lock;
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
	spin_lock_init(&motion_count_lock);
	spin_lock_init(&event_list_lock);
	return 0;
}
static void __exit exitcode(void)
{
	return;
}
int abso(int x)
{
	if (x > 0)
		return x;
	else
		return -x;
}
asmlinkage long sys_accevt_create(struct acc_motion __user *acceleration)
{
	ACC_M *tmpAcc;
	int ret = 0;
	int reteid;

	if (acceleration == NULL)
		return -EINVAL;
	tmpAcc = kmalloc(sizeof(ACC_M), GFP_KERNEL);
	ret = copy_from_user(tmpAcc, acceleration, sizeof(ACC_M));
	if (ret != 0)
		return -EINVAL;
	struct event_unit *new_event;
	new_event = kmalloc(sizeof(E_U), GFP_KERNEL);
	new_event->event_count = 0;
	new_event->mBaseline = *tmpAcc;
	new_event->next = NULL;
	spin_lock(&motion_count_lock);
	motion_count++;
	reteid = motion_count;
	new_event->event_id = motion_count;
	spin_unlock(&motion_count_lock);
	init_waitqueue_head(&new_event->mWaitQueue);
	spin_lock(&event_list_lock);
	if (event_list == NULL) {
		event_list = new_event;
	} else {
		new_event->next = event_list;
		event_list = new_event;
	}
	spin_unlock(&event_list_lock);
	kfree(tmpAcc);
	return reteid;
}
asmlinkage long sys_accevt_wait(int event_id)
{
	struct event_unit *p;
	spin_lock(&event_list_lock);
	p = event_list;
	while (p != NULL) {
		if (p->event_id == event_id)
			break;
		else
			p = p->next;
	}
	if (p == NULL) {
		spin_unlock(&event_list_lock);
		return -EINVAL;
	}
	p->event_count++;
	spin_unlock(&event_list_lock);
	wait_event_interruptible(p->mWaitQueue, condition == event_id);
	condition = 0;
	spin_lock(&event_list_lock);
	p->event_count--;
	spin_unlock(&event_list_lock);
	return 0;
}
asmlinkage long sys_accevt_signal(struct dev_acceleration __user *acceleration)
{
	int ret;
	struct dev_acceleration *tmpACC;
	struct dev_acceleration  *peek_first;
	struct event_unit *p;
	ACC_M *tmpMotion;
	spin_lock(&event_list_lock);
	p = event_list;
	spin_unlock(&event_list_lock);
	tmpACC = kmalloc(sizeof(DEV_A), GFP_KERNEL);
	peek_first = kmalloc(sizeof(DEV_A), GFP_KERNEL);
	/*before this is just malloc, we need to free this
	tmplast is the one that before this, static last make it possible */
	ret = copy_from_user(tmpACC, acceleration, sizeof(DEV_A));

	if (ret != 0)
		return -EINVAL;

	kfifo_put(&acc_kfifo, tmpACC);
	ret = kfifo_len(&acc_kfifo);
	int delte_x = tmpACC->x - last.x;
	int delte_y = tmpACC->y - last.y;
	int delte_z = tmpACC->z - last.z;
	if (abso(delte_x) + abso(delte_y) + abso(delte_z) >= NOISE) {
		total.x += abso(tmpACC->x - last.x);
		total.y += abso(tmpACC->y - last.y);
		total.z += abso(tmpACC->z - last.z);
		totalFrq++;
	}
	if (ret == WINDOW) {
		spin_lock(&event_list_lock);
		while (p != NULL) {
			int tmpX = total.x - p->mBaseline.dlt_x;
			int tmpY = total.y - p->mBaseline.dlt_y;
			int tmpZ = total.z - p->mBaseline.dlt_z;
			int tmpFrq = totalFrq - p->mBaseline.frq;
			if (tmpX > 0 && tmpY > 0 && tmpZ > 0 && tmpFrq > 0) {
				condition = p->event_id;
				wake_up_interruptible_all(&p->mWaitQueue);
				}
			p = p->next;
		}
		spin_unlock(&event_list_lock);
		/*tmpMotion->dlt_x = total.x;
		//tmpMotion->dlt_y = total.y;
		//tmpMotion->dlt_z = total.z;
		//tmpMotion->frq = totalFrq;*/
		kfifo_get(&acc_kfifo, &head);
		kfifo_peek(&acc_kfifo, peek_first);

		int result_x = peek_first->x - head.x;
		int result_y = peek_first->y - head.y;
		int result_z = peek_first->z - head.z;
		if (abso(result_x) + abso(result_y) + abso(result_z) >= NOISE) {
			total.x = total.x - abso(peek_first->x - head.x);
			total.y = total.y - abso(peek_first->y - head.y);
			total.z = total.z - abso(peek_first->z - head.z);
			totalFrq--;
		}
	}
	memcpy(&last, tmpACC, sizeof(DEV_A));
	kfree(tmpACC);
	/*kfree(tmpMotion);*/
	kfree(peek_first);
	return ret;
}
asmlinkage long sys_accevt_destroy(int event_id)
{
	struct event_unit *p, *pre;
	spin_lock(&event_list_lock);
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
	if (p == NULL) {
		spin_unlock(&event_list_lock);
		return -EINVAL;
	}
	if (p->event_count >= 1) {
		/*p->event_count--;*/
		spin_unlock(&event_list_lock);
	} else {
		if (pre == NULL)
			event_list = NULL;
		else
			pre->next = p->next;
		spin_unlock(&event_list_lock);
		kfree(p);
	}
	return 0;
}
module_init(initcode);
module_exit(exitcode);
