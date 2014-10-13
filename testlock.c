#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
struct acc_motion {
	int dlt_x;
	int dlt_y;
	int dlt_z;
	int frq;
};
int main()
{
	int eid[3];
	int i = 0;
	int pid[3];

	struct acc_motion motion1;
	motion1.dlt_x = 3;
	motion1.dlt_y = 1;
	motion1.dlt_z = 1;
	motion1.frq = 2;
	struct acc_motion motion2;
	motion2.dlt_x = 1;
	motion2.dlt_y = 3;
	motion2.dlt_z = 1;
	motion2.frq = 2;
	struct acc_motion motion3;
	motion3.dlt_x = 1;
	motion3.dlt_y = 1;
	motion3.dlt_z = 3;
	motion3.frq = 2;
	eid[0] = syscall(379, &motion1);
	eid[1] = syscall(379, &motion2);
	eid[2] = syscall(379, &motion3);

	while(i < 4) {
		pid[i] = fork();
		if (pid[i] == 0)
		{
			syscall(380,eid[i]);
			printf("shake detect: i = %d, eidi = %d\n",i,eid[i]);
		} else {
			syscall(380,eid[2]);
			printf("shake detect: i = %d, eidi = %d\n",i,eid[i]);
		}
		i++;
	}
	syscall(382,eid[0]);
	syscall(382,eid[1]);
	syscall(382,eid[2]);
	return 0;
}
