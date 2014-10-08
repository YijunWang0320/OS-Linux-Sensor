#include<unistd.h>
#include<sys/syscall.h>
#include<stdio.h>
int main()
{
	int i;
	i=syscall(378,NULL);
	printf("yes!,%d\n",i);
	return 0;
}
