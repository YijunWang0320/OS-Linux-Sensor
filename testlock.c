#include <sys/syscalls.h>
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
	motion1.dlt_x = 5;
	motion1.dlt_y = 5;
	motion1.dlt_z = 5;
	motion1.frq = 10;
	struct acc_motion motion2;
	motion1.dlt_x = 10;
	motion1.dlt_y = 7;
	motion1.dlt_z = 8;
	motion1.frq = 12;
	struct acc_motion motion3;
	motion1.dlt_x = 3;
	motion1.dlt_y = 3;
	motion1.dlt_z = 3;
	motion1.frq = 8;
	eid[0] = syscall(379, motion1);
	eid[1] = syscall(379, motion2);
	eid[2] = syscall(379, motion3);

	while(i < 3) {
		pid[i] = fork();
		if (pid[i] = 0)
		{
			syscall(381,eid[i]);
		} else {
			syscall(381,eid[2]);
		}
		i++;
	}
	syscall(382,eid[0]);
	syscall(382,eid[1]);
	syscall(382,eid[2]);
	return 0;
}