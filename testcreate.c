#include<sys/syscall.h>
#include<stdio.h>
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
	int i;
	int x;
	int y;
	int z;
	int j;

	x = 0;
	y = 0;
	z = 0;
	
	struct dev_acceleration *tmp;
	tmp = (struct dev_acceleration*)malloc(sizeof(struct dev_acceleration));
	for(i=0 ; i<10 ;i++)
	{
		tmp->x=x;
		tmp->y=y;
		tmp->z=z;
		j = syscall(381,tmp);
		printf("the thing: %d\n",tmp->x);
		x++;
		y++;
		z++;
		printf("the number of things in kfifo : %d\n",j);
	}
	return 0;
}
