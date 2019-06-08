/*
*Function:ubuntu下信号量解决消费者生产者问题，使用共享内存作为共享缓冲区,为消费者程序
*Author:ManUtd
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
	int i,j,k;
	int shm_id;
	sem_t *mutex,*full,*empty;
	int* buffer = NULL;
	int out = 0;

	if((shm_id = shm_open("buffer",O_RDWR,0)) < 0)   //打开共享缓冲区
	{
		printf("Error: Fail to open buffer!\n");
		exit(1);
	}
    mutex = sem_open("mutex",O_CREAT,0644,1);  /*如果信号量已经创建，则此处的O_CREAT不会再次创建，若生产者还未创建，此处才创建。以防出错*/  
	full = sem_open("full",O_CREAT,0644,0);    /*数字资源信号量*/
	empty = sem_open("empty",O_CREAT,0644,BUFFER_SIZE);  /*空格信号量*/
	buffer = (int *)mmap(NULL,BUFFER_SIZE*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,shm_id,0);  //映射共享缓冲区
	
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
	
    sem_close(mutex);
    sem_close(full);
	sem_close(empty);
	sem_unlink("mutex");
	sem_unlink("full");
	sem_unlink("empty");

	return 0;
		
    

}
