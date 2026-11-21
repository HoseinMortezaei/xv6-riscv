#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// CPU-bound workload lasting approximately target_ticks clock ticks
void
burn_cpu(int target_ticks)
{
  int start = uptime();
  volatile uint64 counter = 0;
  while (uptime() - start < target_ticks) {
    counter++;
  }
}

int
main(int argc, char *argv[])
{
  printf("=== MLFQ Scheduler Test ===\n");

  // Test 1: Queue 3 (FCFS) & 800-tick Aging Promotion
  // Spawn 3 CPU-bound children; all start in Queue 3 (FCFS).
  // Child 0 runs non-preemptively for ~900 ticks while Children 1 & 2 wait.
  // After 800 wait ticks, Children 1 & 2 age up from Queue 3 to Queue 2!
  printf("\n[Test 1] Spawning 3 CPU-bound children (FCFS + 800-tick aging)...\n");
  for (int i = 0; i < 3; i++) {
    int pid = fork();
    if (pid == 0) {
      int born = uptime();
      if (i == 0) {
        // Long job: holds CPU in Queue 3 long enough to trigger aging in siblings
        burn_cpu(900);
      } else {
        // Shorter jobs: age to Queue 2 while waiting, then compete via SJF
        setsjf(i == 1 ? 10 : 2, 90);
        burn_cpu(40);
      }
      printf("Child %d (pid %d) finished | start=%d end=%d\n",
             i, getpid(), born, uptime());
      exit(0);
    }
  }

  for (int i = 0; i < 3; i++) {
    wait(0);
  }

  // Test 2: Queue 2 SJF Ordering (Short burst vs Long burst)
  // Spawn a blocker in Queue 3 for 850 ticks so two Queue-2 candidates
  // both age into Queue 2 at roughly the same time with different burst_times.
  printf("\n[Test 2] Testing Queue 2 SJF (burst=2 vs burst=15, conf=95)...\n");
  int blocker = fork();
  if (blocker == 0) {
    burn_cpu(850);
    printf("Blocker (pid %d) finished at tick %d\n", getpid(), uptime());
    exit(0);
  }

  int long_job = fork();
  if (long_job == 0) {
    setsjf(15, 95);
    burn_cpu(30);
    printf("SJF Long Job  (burst=15, pid %d) finished at tick %d\n", getpid(), uptime());
    exit(0);
  }

  int short_job = fork();
  if (short_job == 0) {
    setsjf(2, 95);
    burn_cpu(30);
    printf("SJF Short Job (burst=2,  pid %d) finished at tick %d\n", getpid(), uptime());
    exit(0);
  }

  for (int i = 0; i < 3; i++) {
    wait(0);
  }

  printf("\n=== MLFQ Test Complete ===\n");
  exit(0);
}