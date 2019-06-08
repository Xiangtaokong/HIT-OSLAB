#ifndef _SEM_H_
#define _SEM_H_

#ifndef NULL
#define NULL ((void *) 0)
#endif

/*声明信号量API函数*/

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



#endif
