#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
struct acc_motion {
	unsigned int dlt_x;
	unsigned int dlt_y;
	unsigned int dlt_z;
	unsigned int frq;
};
struct dev_acceleration {
	int x;
	int y;
	int z;
};

int main()
{
	struct acc_motion *bl1 = (struct acc_motion*)malloc(sizeof(struct acc_motion));
	bl1->dlt_x = 5;
	bl1->dlt_y = 5;
	bl1->dlt_z = 5;
	bl1->frq = 5;
 	int eid = syscall(379,bl1);
	printf("create eid=%d\n",eid);
	long count = 0;
	while(1){
		int waitret = syscall(380,eid);
		printf("wait fail. wait return=%d\n",waitret);
		printf("motion detected %ld!\n",count);
		count++;
	}
	//printf("event %d destroyed, exit\n");
	return 0;
}
