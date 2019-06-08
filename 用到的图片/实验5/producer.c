/*
*Function:ubuntu下信号量解决消费者生产者问题，使用共享内存作为共享缓冲区,为生产者程序
*Author:
*/

#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

#define BUFFER_SIZE 10

int main()
{
	int i,j,k,in=0;
	int shm_id;
	sem_t *mutex,*full,*empty;
	int* buffer = NULL;

	if((shm_id = shm_open("buffer",O_CREAT|O_RDWR,0644)) < 0)   /*创建共享缓冲区*/
	{
		printf("Error: Fail to creat buffer!\n");
		exit(1);
	}
	ftruncate(shm_id,BUFFER_SIZE*sizeof(int));    /*设置共享缓冲区大小*/
    mutex = sem_open("mutex",O_CREAT,0644,1);    
	full = sem_open("full",O_CREAT,0644,0);    /*数字资源信号量*/
	empty = sem_open("empty",O_CREAT,0644,BUFFER_SIZE);  /*空格信号量*/
	buffer = (int *)mmap(NULL,BUFFER_SIZE*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,shm_id,0);  /*映射共享缓冲区*/
	
	for(i=0;i<BUFFER_SIZE;i++)
		buffer[i] = 0;               /*初始化缓冲区*/
                                     /*创建生产者进程*/
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

    sem_close(mutex);
    sem_close(full);
	sem_close(empty);

	return 0;



	
}
