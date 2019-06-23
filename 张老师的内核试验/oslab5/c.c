#include <sys/time.h>
#include <stdio.h>
#include <signal.h>
static void sighandle(int);
static long realsecond = 0;
static long vtsecond = 0;
static long profsecond = 0;
static struct itimerval realt,virtt,proft;int main(){
struct itimerval v;
int i,j;
long moresec,moremsec,t1,t2;
signal(SIGALRM,sighandle);
signal(SIGVTALRM,sighandle);
signal(SIGPROF,sighandle);
v.it_interval.tv_sec = 10;
v.it_interval.tv_usec = 0;
v.it_value.tv_sec = 10;
v.it_value.tv_usec = 0;
setitimer(ITIMER_REAL,&v,NULL);
setitimer(ITIMER_VIRTUAL,&v,NULL);
setitimer(ITIMER_PROF,&v,NULL);
for(j= 0;j<1000;j++){
for(i= 0;i<500;i++){printf("********\r");fflush(stdout);}
}
getitimer(ITIMER_PROF,&proft);
getitimer(ITIMER_REAL,&realt);
getitimer(ITIMER_VIRTUAL,&virtt);
printf("\n");
moresec = 10 - realt.it_value.tv_sec;
moremsec = (1000000 - realt.it_value.tv_usec)/1000;
printf("realtime = %ld sec, %ld msec\n",realsecond+moresec,moremsec);
moresec = 10 - proft.it_value.tv_sec;
moremsec = (1000000 - proft.it_value.tv_usec)/1000;
printf("cputime = %ld sec, %ld msec\n",profsecond+moresec,moremsec);
moresec = 10 - virtt.it_value.tv_sec;
moremsec = (1000000 - virtt.it_value.tv_usec)/1000;
printf("usertime = %ld sec, %ld msec\n",vtsecond+moresec,moremsec);
t1 = (10 - proft.it_value.tv_sec)*1000 + (1000000 - proft.it_value.tv_usec)/1000 +
profsecond*10000;
t2 = (10 - virtt.it_value.tv_sec)*1000 + (1000000 - virtt.it_value.tv_usec)/1000 +
vtsecond*10000;
moresec = (t1 - t2)/1000;
moremsec = (t1 - t2) % 1000;
printf("kerneltime = %ld sec, %ld msec\n",moresec,moremsec);
fflush(stdout);}
static void sighandle(int s)
{
switch(s){
case SIGALRM:realsecond+=10;break;
case SIGVTALRM:vtsecond+=10;break;
case SIGPROF:profsecond+=10;break;
default :break;
}
}
