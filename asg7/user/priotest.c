#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define CHILDREN 4
#define SEM_OUTPUT 0
#define LOOP_COUNT 250000000

int
main(int argc, char *argv[])
{
  int priorities[CHILDREN] = {1, 5, 10, 20};

  if (sem_init(SEM_OUTPUT, 1) < 0) {
    printf("priotest: sem_init failed\n");
    exit(1);
  }
  printf("Completion order (reported by each child):\n");

  for (int i = 0; i < CHILDREN; i++) {
    int pid = fork();
    if (pid < 0) {
      printf("priotest: fork failed\n");
      exit(1);
    }
    if (pid == 0) {
      int self = getpid();
      int last_priority = get_priority(self);
      for (volatile int n = 0; n < LOOP_COUNT; n++) {
        if ((n % 1000000) == 0) {
          int current = get_priority(self);
          if (current != last_priority) {
            if (sem_wait(SEM_OUTPUT) < 0)
              exit(1);
            printf("pid %d aged to priority %d\n", self, current);
            sem_signal(SEM_OUTPUT);
            last_priority = current;
          }
        }
      }
      if (sem_wait(SEM_OUTPUT) < 0)
        exit(1);
      printf("completed pid %d at priority %d\n", self,
             get_priority(self));
      sem_signal(SEM_OUTPUT);
      exit(0);
    }
    if (set_priority(pid, priorities[i]) < 0) {
      kill(pid);
      wait(0);
      printf("priotest: set_priority failed\n");
      exit(1);
    }
  }

  for (int i = 0; i < CHILDREN; i++) {
    if (wait(0) < 0) {
      printf("priotest: wait failed\n");
      exit(1);
    }
  }
  exit(0);
}
