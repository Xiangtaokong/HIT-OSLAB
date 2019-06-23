#include <sys/time.h>
#include <stdio.h>
#include <signal.h>
static void sighandle(int);
static int second = 0;
int main(){
struct itimerval v;
signal(SIGALRM,sighandle);
v.it_interval.tv_sec = 1;
v.it_interval.tv_usec = 0;
v.it_value.tv_sec = 1;
v.it_value.tv_usec = 0;
setitimer(ITIMER_REAL,&v,NULL);
for(;;);
}
static void sighandle(int s)
{second++;
printf("%d\r",second);
fflush(stdout);
}
