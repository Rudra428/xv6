#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NCHILD 3
#define WORK 10000000

int
main(void)
{
  int i, pid;
  volatile uint64 delay, work;

  printf("FCFS test: children should complete in creation order\n");
  for (i = 0; i < NCHILD; i++) {
    for (delay = 0; delay < 200000; delay++)
      ;
    pid = fork();
    if (pid < 0) {
      printf("FCFS test: fork failed\n");
      exit(1);
    }
    if (pid == 0) {
      for (work = 0; work < WORK; work++)
        ;
      printf("FCFS completion: pid %d (created position %d)\n", getpid(), i + 1);
      exit(0);
    }
  }

  for (i = 0; i < NCHILD; i++)
    wait(0);
  exit(0);
}
