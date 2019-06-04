
/*
 *  一个是iam()：功能为完成将字符串拷贝到内核中保存下来，返回值为拷贝的字符数
 *            要求字符串长度不超过20个字符，否则返回-1,并置errno为EINVAL
 * 另一个是whoami():功能为将内核中由iam()保存的字符串拷贝到name指向的用户地址空间中，返回值为拷贝的字符数
 *                要求确保不会对name越界访存，如果size小于需要的空间，则返回-1,并置errno为EINVAL
 */
#define __LIBRARY__
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <asm/segment.h>

extern char buffe[64]={0};

int sys_iam(const char* name)
{
    printk("Now we are in sys_iam!\n");    

	int i=0;
	
    while(get_fs_byte(name+i)!='\0')
	   i++;
	if(i>20)   
	   return -EINVAL;
	   
    i=0;
    while((buffe[i]=get_fs_byte(name+i)) != '\0' && i<=20)
	   i++;        //成功拷贝一个字符加一次
	
    return i;
}

int sys_whoami(char* name,unsigned int size)
{
    printk("Now we are in sys_whoami!\n"); 

    int i,j;
	
    for(i=0;i<=20;i++)
	{
	   if(buffe[i] == '\0')
	       break;
	}
    if(i>size)
	   return -EINVAL;
	for(j=0;j<i;j++)
	   put_fs_byte(buffe[j],(name+j));
    
    return i;
}
