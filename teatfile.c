#include<unistd.h>
#include<sys/syscall.h>
#include<stdio.h>
#include <stdlib.h>
#include "acceleration_d/acceleration.h"
int main()
{
	int i;
	struct dev_acceleration *acInfo = (struct dev_acceleration*)malloc(sizeof(struct dev_acceleration));
	acInfo->x=1;
	acInfo->y=2;
	acInfo->z=3;
	i=syscall(380,acInfo);
	printf("yes!,%d\n",i);
	free(acInfo);
	return 0;
}
