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

	struct acc_motion motion1;
	motion1.dlt_x = 50;
	motion1.dlt_y = 0;
	motion1.dlt_z = 0;
	motion1.frq = 5;
	struct acc_motion motion2;
	motion2.dlt_x = 0;
	motion2.dlt_y = 0;
	motion2.dlt_z = 50;
	motion2.frq = 5;
	struct acc_motion motion3;
	motion3.dlt_x = 0;
	motion3.dlt_y = 50;
	motion3.dlt_z = 0;
	motion3.frq = 5;
	eid[0] = syscall(379, &motion1);
	eid[1] = syscall(379, &motion2);
	eid[2] = syscall(379, &motion3);
	int i;
	int j;
	pid_t pid;
	for (i=1;i<=4;i++) {
		pid = fork();
		if (pid == 0) {
			printf("I am a child: %d, waiting for: %d\n",i,eid[i%3]);
			while (1) {
			j = syscall(380,eid[i%3]);
			if(i%3 == 0)
				printf("X shake, eid = %d\n", eid[i%3]);
			else if(i%3 == 1)
				printf("Z shake, eid = %d\n", eid[i%3]);
			else
				printf("Y shake, eid = %d\n", eid[i%3]);
			}
			return 0;
		}
	}
	while(wait(0)>0);
	syscall(382,eid[0]);
	syscall(382,eid[1]);
	syscall(382,eid[2]);
	printf("destroyed all 3 motions!\n");
	return 0;
}
