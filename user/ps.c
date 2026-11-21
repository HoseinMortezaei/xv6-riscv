#include <user/user.h>
#include <kernel/param.h>

static char *states[] = {"UNUSED",   "USED",    "SLEEPING",
                         "RUNNABLE", "RUNNING", "ZOMBIE"};

int
main(void)
{

  struct pinfo buf[NPROC];
  int n = getprocs((uint64)buf, NPROC);

  if (n < 0) {
    fprintf(2, "ps: getproc failes\n");
    exit(1);
  }

  printf("PID\tPPID\tSTATE\t\tSIZE\tNAME\n");
  for (int i = 0; i < n; i++) {
    printf("%d\t%d\t%s\t\t%d\t%s\n", buf[i].pid, buf[i].ppid,
           states[buf[i].state], (int)buf[i].sz, buf[i].name);
  }
  exit(0);
}
