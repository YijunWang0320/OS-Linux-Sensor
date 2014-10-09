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
		printf("%d",j);
		x++;
		y++;
		z++;
	}
	return 0;
}
