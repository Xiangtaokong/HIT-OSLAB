#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h> 
#include <fcntl.h> 
#include <sys/wait.h>
#define PIPLEN 100
sem_t sem;//semaphore
//pipe:FIFO

int pipeSize(){//measure size of pipe
  int ret,count=0,filedes[2];  
  pipe(filedes);
    fcntl(filedes[1],F_SETFL,O_NONBLOCK); //set the file state to nonblock ,nonblock writing
    while(1)  
    {  
        ret=write(filedes[1],"1",1);//write  
        if(ret==-1)  break;   //full
        count++;  
    }  
    printf("The length of unamed pipe is:%dB\n\n",count);
    close(filedes[0]);
    close(filedes[1]);
    return count;
}

void err(){
    printf("fork error!\n"); 
    exit(0);
}

int main()
{
   int fd[2],p1,p2,p3,len,actLen;      //fd[0] for read,1 for write
   char outpipe[PIPLEN],inpipe[PIPLEN];
   int maxSize=pipeSize();
   // int availableLen=maxSize;
   pipe(fd);                     //create a unamed pipe
   sem_init(&sem,1,1);//second 1:multiple processes,init sem=1
   if((p1=fork())==-1){
      err();
    }       
   if(p1==0)
   {
      sem_wait(&sem);
      close(fd[0]);       //writing,close read
      sprintf(outpipe,"This is test for child1!\n");         
      len=strlen(outpipe);
      printf("child1 process wants to write %d bytes\n",len);
      actLen=write(fd[1],outpipe,len);  //actually write in
      printf("child1 process actually writes %d bytes\n",actLen);
      //availableLen-=actLen;
      sleep(1);       //wait for a second
      sem_post(&sem);
      // if (availableLen<0)
      // {
      //    sem_post(full);//full,block reading process
      // }
      exit(0);                //child process ends
   }
   else//father
   {
       if((p2=fork())==-1){
            err();
       }
       if(p2==0)
       {
          sem_wait(&sem);
          close(fd[0]);       //writing,close read
          sprintf(outpipe,"This is test for child2!\n");         
          len=strlen(outpipe);
          printf("child2 process wants to write %d bytes\n",len);
          actLen=write(fd[1],outpipe,len);  //actually write in
          printf("child2 process actually writes %d bytes\n",actLen);
          sleep(1);
          sem_post(&sem);
          exit(0);     
       }
        else//father
        {
           if((p3=fork())==-1){
              err();
           }
           if(p3==0)
           {
              sem_wait(&sem);
              close(fd[0]);       //writing,close read
              sprintf(outpipe,"This is test for child3!\n");         
              len=strlen(outpipe);
              printf("child3 process wants to write %d bytes\n",len);
              actLen=write(fd[1],outpipe,len);  //actually write in
              printf("child3 process actually writes %d bytes\n",actLen);
              sleep(1);
              sem_post(&sem);
              exit(0);     
           }
            else//father
           {
              wait(0);           //wait for a child process
              read(fd[0],inpipe,PIPLEN);
              printf("%s\n",inpipe);
              wait(0);           //wait for a child process
              read(fd[0],inpipe,PIPLEN);
              printf("%s\n",inpipe);
              wait(0);           //wait for a child process
              read(fd[0],inpipe,PIPLEN);
              printf("%s\n",inpipe);
              exit(0);
           }
         }
     }
     return 0;
}
