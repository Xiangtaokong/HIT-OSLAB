#define __LIBRARY__
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
//kxt

_syscall2(int,whoami,char*,name,unsigned int,size)

int main()
{
    int i,counter;
	char buf[64]={'\0'};
	
    counter=whoami(buf,64);
	if(counter < 0)
	{
	   printf("whoami:error!");
	   return -1;
    }
	
	for(i=0;i<counter;i++)
	   printf("%c",buf[i]);
	printf("\n");
    
    return 0;
}
