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
	bl1->dlt_x = 3;
	bl1->dlt_y = 1;
	bl1->dlt_z = 1;
	bl1->frq = 2;
 	int eid = syscall(379,bl1);
	int waitret = syscall(380,eid);
	printf("motion detected ahhhhhhaaaaaaa!\n");
	return 0;
}
