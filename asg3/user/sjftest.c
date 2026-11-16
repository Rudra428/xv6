#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NCHILD 4

int
main(void)
{
  int i, pid;
  int bursts[NCHILD] = {40, 10, 30, 20};

  printf("SJF test: children should complete in ascending burst-time order\n");
  for (i = 0; i < NCHILD; i++) {
    pid = fork();
    if (pid < 0) {
      printf("SJF test: fork failed\n");
      exit(1);
    }
    if (pid == 0) {
      volatile uint64 work;

      for (work = 0; work < (uint64)bursts[i] * 100000; work++)
        ;
      printf("SJF completion: pid %d burst %d\n", getpid(), bursts[i]);
      exit(0);
    }
    if (set_burst(pid, bursts[i]) < 0) {
      printf("SJF test: set_burst failed\n");
      exit(1);
    }
  }

  for (i = 0; i < NCHILD; i++)
    wait(0);
  exit(0);
}
