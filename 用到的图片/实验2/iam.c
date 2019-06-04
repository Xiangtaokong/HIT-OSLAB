#define __LIBRARY__
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

_syscall1(int,iam,const char*,name)

int main(int argc,char* argv[])
{
    int i;
	char buf[1024]={'\0'};
	
    if(argc>1)
	{
	   for(i=1;i<argc;i++)
	   {
	      strcat(buf,argv[i]);
	   }
	}
    
    if(iam(buf) < 0)
	{
	   printf("iam:error!");
	   return -1;
	}
	
    return 0;

}