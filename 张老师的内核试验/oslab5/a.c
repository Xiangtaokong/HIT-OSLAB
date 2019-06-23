#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
int num=0;
void realtimer_set(){
struct itimerval v,oldtv;
v.it_interval.tv_sec = 1;
v.it_interval.tv_usec = 0;
v.it_value.tv_sec = 1;
v.it_value.tv_usec = 0;
setitimer(ITIMER_REAL,&v,&oldtv);
}
void sig(int sig)
{
num++;
}
int main(){
signal(SIGALRM,sig);
realtimer_set();
printf("you have only 10s\n");
while(num<10)
{}
printf("now time is up!\n");
exit(0);
}
