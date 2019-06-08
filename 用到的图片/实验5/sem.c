#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <asm/system.h>
#include <string.h>

/*实现自己的等待队列和ADT*/
/*等待队列的原型在unistd.h中*/

void makenull(wait_queue* Q)
{
  int i;
  Q->front = 0;
  Q->rear = QUEUE_SIZE-1;
  for(i=0;i<QUEUE_SIZE;i++)
     Q->pro[i] = NULL;
}
int empty(wait_queue Q)
{
   if(((Q.rear+1)%QUEUE_SIZE) == Q.front)
      return 1;
   else
      return 0;
}
struct task_struct* getfront(wait_queue Q)
{
   if(empty(Q))
     return NULL; 
   else
     return (Q.pro[Q.front]);
}
void enqueue(struct task_struct* p,wait_queue* Q)
{
	if(((Q->rear+2)%QUEUE_SIZE) == Q->front)
	{
		/*printk("Error:wait_queue is full!\n"); */
	   _exit(1);
	}
	else{
		Q->rear = (Q->rear+1)%QUEUE_SIZE;
		Q->pro[Q->rear] = p;
	}
}
void dequeue(wait_queue* Q)
{
	if(!empty(*Q))
	   Q->front = (Q->front+1)%QUEUE_SIZE;
}
/*实现完毕等待队列*/

/*此后为信号灯的系统调用处理函数实现*/
/*信号灯的定义在unistd.h中*/

char* sem_name[64]={};
sem_t sem[64]={};
char name_array[64][128]={};
wait_queue wait_Q[64];

extern void sleep_on_queue();
extern void wake_up_queue(struct task_struct **p);
extern int mystrcmp(const char* str1,const char* str2);

int sys_sem_open(const char* name,unsigned int value)
{
	char nametmp[128]={'\0'};
    int j,flag;
	int i=0;

	if(get_fs_byte(name) == '\0')   /*进行name名检测，防止空字符串传入*/
		return -1;

	while((nametmp[i] = get_fs_byte(name+i)) != '\0')
		i++;
	                                         /*退出循环时得到的i为字符串字符个数*/
	flag = 0;    /*信号量是否已经存在的标志*/
	for(j=0;j<64;j++)
	{
		if(!mystrcmp(sem_name[j],nametmp))
		{
			flag = 1;   /*说明要创建的信号量已存在*/
            break;
		}
	}
	if(flag == 1)
		return (&sem[j]);  /*直接返回信号量的指针*/
	else                   /*信号量不存在，需要新创建一个*/
	{
	    for(j=0;j<64;j++)
	    {
		  if(name_array[j][0] == '\0')
		  {
			sem_name[j] = strcpy(name_array[j],nametmp);  /*j对应的是创建的信号量的项号*/
			flag = 1;
			break;
		  }
	      if(j == 63)
		  {
			printk("Error:too many sem!\n");
		    return -1;   
		  }
	    }
	    if(flag == 1)
		{
			sem[j].value = value;
			sem[j].queue = &wait_Q[j];
			makenull(sem[j].queue);     //初始化信号量对应的等待队列
			return (&sem[j]);
		}
		
		return -1;
	}
}

int sys_sem_unlink(const char *name)
{
	char nametmp[128]={'\0'};
	int j,k,flag;
	int i = 0;

	if(get_fs_byte(name) == '\0') /*进行name名检测，防止空字符串传入*/
		return (-1);
	
	while((nametmp[i] = get_fs_byte(name+i)) != '\0')
		i++;
	                                         /*退出循环时得到的i为字符串字符个数*/
	flag=0;    /*信号量是否已经存在的标志*/
	for(j=0;j<64;j++)
	{
		if(!mystrcmp(sem_name[j],nametmp))
		{
			flag = 1;   /*说明要关闭的信号量已存在*/
            break;
		}
	}
	if(flag == 1)
	{
		for(k=0;k<128;k++)
			name_array[j][k] = '\0';
		sem_name[j] = NULL;
		sem[j].value = 0;
		sem[j].queue = NULL;
		return 0;
	}
	else
		return -1;
}

int sys_sem_wait(sem_t* sema)
{
	if(sema == NULL)
		return -1;

	cli();
	sema->value--;
	if((sema->value) < 0)
	{
		enqueue(current,sema->queue);
        sleep_on_queue();
		sti();
		return 0;
	}
	else
	{
		sti();
		return 0;
	}
}
	
int sys_sem_post(sem_t* sema)
{
	if(sema == NULL)
		return -1;

	cli();
	sema->value++;
	if((sema->value) <= 0)
	{
		struct task_struct* front;
		front = getfront(*(sema->queue));
		dequeue(sema->queue);
		wake_up_queue(&front);
	}
	sti();
	return 0;
}

int sys_sem_getvalue(sem_t* sema,int* valsem)      /*此处极易出错，因为是需要把内核中的内容赋值到用户空间中去，注意，参数传过来的地址是用户态的地址！*/
{
	if(sema == NULL)
		return -1;

	put_fs_long(sema->value,valsem);
    return 0;
}

void sleep_on_queue()
{
    /*if(current == &(init_task.task)) 
		panic("task[0] trying to sleep!");*/

	current->state = TASK_UNINTERRUPTIBLE;
	schedule();
}

void wake_up_queue(struct task_struct **p)
{
	if(p&& *p)
	{
		(**p).state = TASK_RUNNING;
	}
}

int mystrcmp(const char* str1,const char* str2)   /*只判断两个字符串是否相等,相等返回0，不相等返回1，错误返回-1*/
{
int i=0,j=0;

	if(str1 == NULL || str2 == NULL)
		return -1;

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
	
	
	
			
			
	  
	
    
	




