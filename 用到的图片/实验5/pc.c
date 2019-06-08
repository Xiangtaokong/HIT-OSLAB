/*
*Function:linux0.11下信号量解决消费者生产者问题，使用共享内存作为共享缓冲区,为生产者程序
*Author:
*/

#define __LIBRARY__            /*切记：这个一定要加，否则unistd.h中的宏无法使用，切记！*/
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFER_SIZE 10

_syscall1(key_t,ftok,const char *,name)
_syscall3(int,shmget,key_t,key,size_t,size,int,shmflg)
void* shmat(int shmid,const void * shmaddr,int shmflg)
{
	long _res;
	__asm__ volatile ("int $0x80"
	                  : "=a" (_res)
	                  : "0" (__NR_shmat),"b" ((long)(shmid)),"c" ((long)(shmaddr)),"d" ((long)(shmflg)));
	
	   return (void *)_res;
}
sem_t* sem_open(const char *name,int ignore1,int ignore2,unsigned int value)
{
	long _res;
	__asm__ volatile ("int $0x80"
	                  : "=a" (_res)
	                  : "0" (__NR_sem_open),"b" ((long)(name)),"c" ((long)(value)));
	
	   return (sem_t *)_res;
}
_syscall1(int,sem_unlink,const char *,name)
_syscall1(int,sem_wait,sem_t *,sema)
_syscall1(int,sem_post,sem_t *,sema)
_syscall2(int,sem_getvalue,sem_t *,sema,int *,valsem)

int main()
{
	int i,j,k,in=0;
	int shm_id;
	sem_t *mutex,*full,*empty;
	int* buffer = NULL;
        key_t shm_key;
	int out = 0;
        
        if((shm_key = ftok("buffer")) < 0)
        {
		printf("Error: Fail to creat key!\n");
		exit(1);
	}
	if((shm_id = shmget(shm_key,BUFFER_SIZE*sizeof(int),0)) < 0)   /*创建共享缓冲区*/
	{
		printf("Error: Fail to creat buffer!\n");
		exit(1);
	}
        mutex = sem_open("mutex",O_CREAT,0644,1);    
	full = sem_open("full",O_CREAT,0644,0);    /*数字资源信号量*/
	empty = sem_open("empty",O_CREAT,0644,BUFFER_SIZE);  /*空格信号量*/
	buffer = (int*)shmat(shm_id,NULL,0);  /*映射共享缓冲区*/
	
	for(i=0;i<BUFFER_SIZE;i++)
		buffer[i] = 0;               /*初始化缓冲区*/
                                     /*创建生产者进程*/
	if(!fork())  /*创建生产者进程*/
	{
buffer = (int*)shmat(shm_id,NULL,0); 
		in = 0;
		for(j=0;j<500;j++)
		{
			sem_wait(empty);
			sem_wait(mutex);
			buffer[in] = j;
			in = (in+1) % BUFFER_SIZE;  
			sem_post(mutex);
			sem_post(full);
		}
		exit(0);
	}

	if(!fork())
	{
buffer = (int*)shmat(shm_id,NULL,0); 
           out = 0;
	   for(k=0;k<500;k++)
	   {
		sem_wait(full);
		sem_wait(mutex);
		printf("%ld: %d\n",(long)getpid(),buffer[out]);
                fflush(stdout);
		buffer[out] = 0;
		out = (out+1) % BUFFER_SIZE;
		sem_post(mutex);
		sem_post(empty);
	   }
           exit(0);
        }

	wait(NULL);


	return 0;
}
