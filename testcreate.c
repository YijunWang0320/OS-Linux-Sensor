#include<sys/syscall.h>
#include<stdio.h>
struct acc_motion {
	unsigned int dlt_x;
	unsigned int dlt_y;
	unsigned int dlt_z;
	unsigned int frq;
};
int main()
{
	int x;
	int y;
	int z;
	int i;
	int j;
	struct acc_motion tmp;

	x=1;
	y=1;
	z=1;
	i=0;
	for(i = 0;i < 11; i++)
	{
		tmp.dlt_x=x;
		tmp.dlt_y=y;
		tmp.dlt_z=z;
		tmp.frq=x+y+z;
		j = syscall(379,&tmp);
		printf("before destroy%d\n",j);
		x++;
		y++;
		z++;
	}

	for (i = 4;i<8;i++)
	{
		j = syscall(381,NULL);
		printf("After destroy, return=%d\n",j);
	}
	return 0;
}
