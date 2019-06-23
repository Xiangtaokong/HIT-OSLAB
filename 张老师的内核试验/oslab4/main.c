#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  char repTypeName[16];
  char c1, c2, ch;
  int interval, duration;
  char *lineBuf;
  int LB_SIZE = 255; // 设置最大长度
  FILE *thisProcFile;
  struct timeval now; // 更正类型
  int iteration;

  lineBuf = (char *)malloc(sizeof(char) * LB_SIZE); // 分配空间
  interval = 2; // 初始化参数
  duration = 60;
  strcpy(repTypeName, "Standard"); 
  if(argc > 1) { // 更正, Linux下argc至少为1, 故改为 > 1, 即没有参数时越过
    sscanf(argv[1], "%c%c", &c1, &c2);
    if(c1 != '-') {
      fprintf(stderr, "usage: ksamp [-s][-l int dur]\n");
      exit(1);
    }
    if(c2 == 's')
      strcpy(repTypeName, "Short");
    if(c2 == 'l') {
      strcpy(repTypeName, "Long");
      interval = atoi(argv[2]);
      duration = atoi(argv[3]);
    }
  }
// 获取时间
  gettimeofday(&now, NULL);
  printf("Status report %s at %s\n", repTypeName, ctime(&(now.tv_sec)));
// 更正ctime函数调用
// 读取hostname
  thisProcFile = fopen("/etc/hostname", "r"); 
  memset(lineBuf, 0, LB_SIZE); // 初始化lineBuf
  fgets(lineBuf, LB_SIZE - 1, thisProcFile);
  printf("Machine hostname: %s\n", lineBuf);
  fclose(thisProcFile);
  // 读取版本信息
  thisProcFile = fopen("/proc/version", "r");
  memset(lineBuf, 0, LB_SIZE);
  fgets(lineBuf, LB_SIZE - 1, thisProcFile);
  printf("The Version: %s\n", lineBuf);
  fclose(thisProcFile);
  // 读取CPU信息
  thisProcFile = fopen("/proc/cpuinfo", "r");
  memset(lineBuf, 0, LB_SIZE);
  fgets(lineBuf, LB_SIZE - 1, thisProcFile);
  printf("The Cpu: %s\n", lineBuf);
  fclose(thisProcFile);
  // 读取当前时间
  thisProcFile = fopen("/proc/uptime", "r");
  memset(lineBuf, 0, LB_SIZE);
  fgets(lineBuf, LB_SIZE - 1, thisProcFile);
  printf("The Running Time: %s\n", lineBuf);
  fclose(thisProcFile);
  // 读取内存信息
  printf("The MemInfo: ");
  thisProcFile = fopen("/proc/meminfo", "r");
  while(!feof(thisProcFile)) {
    putchar(fgetc(thisProcFile));
  }
  fclose(thisProcFile);
  printf("The Status: ");
  thisProcFile = fopen("/proc/stat", "r");
  while(!feof(thisProcFile)) {
    putchar(fgetc(thisProcFile));
  }
  fclose(thisProcFile);
  iteration = 0;
  while(iteration < duration) {
    sleep(interval);
    thisProcFile = fopen("/proc/loadavg", "r");
    while(!feof(thisProcFile)) {
      putchar(fgetc(thisProcFile));
    }
    fclose(thisProcFile);
    iteration += interval;
  }
  return 0;
}
