#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int pids[3];

  for(int i = 0; i < 3; i++){
    int pid = fork();
    if(pid == 0){
      if(i == 1) pause(200);
      else       for(;;) ;
      exit(0);
    }
    pids[i] = pid;
  }

  pause(20);

  struct pinfo buf[64];

  printf("--- full list (max=64) ---\n");
  int n = getprocs((uint64)buf, 64);
  printf("returned %d\n", n);
  for(int i = 0; i < n; i++)
    printf("pid=%d ppid=%d state=%d name=%s\n",
           buf[i].pid, buf[i].ppid, buf[i].state, buf[i].name);

  printf("--- max=2 ---\n");
  n = getprocs((uint64)buf, 2);
  printf("returned %d\n", n);

  printf("--- maxn---\n");
  printf("max=0  -> %d\n", getprocs((uint64)buf, 0));
  printf("max=-5 -> %d\n", getprocs((uint64)buf, -5));

  for(int i = 0; i < 3; i++){
    int found = 0;
    for(int j = 0; j < 64; j++){
      if(buf[j].pid == pids[i]) found = 1;
    }
    printf("child %d %s\n", pids[i], found ? "found" : "not found");
  }

  exit(0);
}