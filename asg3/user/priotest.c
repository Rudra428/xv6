#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NCHILD 4
#define AGING_TEST_TICKS 120
#define WORK 10000000

int
main(void)
{
  int i, pid;
  int priorities[NCHILD] = {1, 20, 19, 4};

  if (set_priority(getpid(), 1) < 0) {
    printf("Priority test: set_priority failed\n");
    exit(1);
  }
  printf("Priority test: pid, priority, start/completion order\n");
  for (i = 0; i < NCHILD; i++) {
    pid = fork();
    if (pid < 0) {
      printf("Priority test: fork failed\n");
      exit(1);
    }
    if (pid == 0) {
      volatile uint64 work;
      uint start;

      printf("Priority start: pid %d priority %d\n", getpid(), priorities[i]);
      if (i == 0 || i == NCHILD - 1) {
        start = uptime();
        while (uptime() - start < AGING_TEST_TICKS) {
          for (work = 0; work < WORK; work++)
            ;
        }
      } else {
        for (work = 0; work < WORK; work++)
          ;
      }
      printf("Priority completion: pid %d priority %d\n", getpid(),
             priorities[i]);
      exit(0);
    }
    if (set_priority(pid, priorities[i]) < 0) {
      printf("Priority test: set_priority failed\n");
      exit(1);
    }
  }

  for (i = 0; i < NCHILD; i++)
    wait(0);
  exit(0);
}
