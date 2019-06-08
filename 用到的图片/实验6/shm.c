#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <asm/system.h>
#include <string.h>
#include <sys/types.h>

#define LOW_MEM 0x100000
#define PAGING_MEMORY (15*1024*1024)
#define PAGING_PAGES (PAGING_MEMORY>>12)
#define PAGE_SIZE 4*1024

/*此后为共享内存的系统调用处理函数实现*/
/*key_t的定义在unistd.h中*/

int shm_id[64]={};
char shm_name[64][128]={};

key_t sys_ftok(const char* name)
{
	char nametmp[128]={'\0'};
    int j,flag;
	int i=0;

	if(get_fs_byte(name) == '\0')   /*进行name名检测，防止空字符串传入*/
		return -1;

	while((nametmp[i] = get_fs_byte(name+i)) != '\0')
		i++;
	                                         /*退出循环时得到的i为字符串字符个数*/
	flag = 0;    /*初始化共享内存是否已经存在的标志*/
	for(j=0;j<64;j++)
	{
		if(!mystrcmp(shm_name[j],nametmp))
		{
			flag = 1;     /*说明要创建的共享内存已存在*/
            break;
		}
	}
	if(flag == 1)
		return ((key_t)j);  /*直接返回共享内存的key,即共享内存shm_id数组的项号*/
	else                   /*共享内存不存在*/
	{
	    for(j=0;j<64;j++)
	    {
		  if(shm_name[j][0] == '\0')
		  {
			strcpy(shm_name[j],nametmp); 
			return ((key_t)j);
		  }
	      if(j == 63)
		  {
			printk("Error:too many shm!\n");
		    return -1;   
		  }
	    }
		
		return -1;
	}
}

int sys_shmget(key_t key,size_t size,int shmflg)
{
	unsigned long page = 0;

	if(size > PAGE_SIZE)
	{
		printk("Error:size is too big!\n");
		return -EINVAL;
	}

	if(shm_id[key] >= 896 && shm_id[key] < PAGING_PAGES)
		return shm_id[key];

	if(shm_id[key] == 0)                   /*共享内存不存在，需要新创建一个*/
	{
		
		if(!(page = get_free_page()))    /*新创建一个内存页面用于共享*/
		{
		    printk("Error:memory is full!\n");
		    return -ENOMEM;   
		}	
  printk("mem_map(get): %d\n",get_mem_map(page));
		if(page < LOW_MEM)   /*注意！！！：此处后一个判断有待考察！！！*/
		{
		    printk("Error:page address is invalid!\n");
		    return -EINVAL;
	    }	
		shm_id[key] = (page - LOW_MEM) >> 12;         /*合法的页面起始地址，根据它换算出页面号*/
		if(shm_id[key] >= 896 && shm_id[key] < PAGING_PAGES)
		    return shm_id[key];
	}

	printk("Error:get shm failed!\n");
	return -EINVAL;
}

void* sys_shmat(int shmid,const void *shmaddr,int shmflag)
{
	unsigned long off = 0;
	int i = 0;
	unsigned long L,addr;

	if(shmid < 896 || shmid >= PAGING_PAGES)   /*非法shmid，进行错误处理*/
	{
	    printk("Error:shmid is invalid!\n");
	    return -EINVAL;
	}

	L = get_base(current->ldt[1]) + current->brk + 10*4096;
  /*L = get_base(current->ldt[1]) + current->brk + 10*4096 + PAGE_SIZE-1;
	L = L & 0xFFFFF000;*/                         /*如此计算出的线性地址L能保证在页面地址边缘*/	
    addr = (shmid << 12) + LOW_MEM;   /*换算出共享内存页面的起始地址*/
        printk("mem_map(at): %d\n",get_mem_map(addr));
	if(!put_page(addr,L))
	{
	    printk("Error:put page failed!\n");
	    return -EINVAL;
	}
	printk("after put_page,mem_map(at): %d\n",get_mem_map(addr));
	off = L - get_base(current->ldt[1]); 
    printk("offset: %ld\n",off);
	return off;
}

void sys_shmdt(key_t key)
{
    int i;

    if(key<0 || key >= 64)
      _exit(1);
    
    shm_id[key] = 0;
    for(i=0;i<128;i++)
       shm_name[key][i] = '\0';
}

extern int mystrcmp(const char* str1,const char* str2)   /*只判断两个字符串是否相等,相等返回0，不相等返回1，错误返回-1*/
{
	if(str1 == NULL || str2 == NULL)
		return -1;

	int i=0,j=0;

	while((*(str1+i)) != '\0')
	{
		if((*(str1+i)) != (*(str2+j)))
			return 1;
		i++;
		j++;
	}
	if((*(str2+j)) == '\0')
		return 0;
	else
		return 1;
}
	
	
	
			
			
	  
	
    
	




