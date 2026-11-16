#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

volatile int counter = 0;

// 3 page-aligned stacks in global memory (no lazy malloc issues!)
char stack1[4096] __attribute__((aligned(4096)));
char stack2[4096] __attribute__((aligned(4096)));
char stack3[4096] __attribute__((aligned(4096)));

void
worker(void *arg)
{
  for (int i = 0; i < 100000; i++) {
    counter++;
  }
  exit(0);
}

int
main(int argc, char *argv[])
{
  int i, created = 0;
  char *stacks[] = {stack1, stack2, stack3};

  printf("Starting 3 threads...\n");

  for (i = 0; i < 3; i++) {
    if (clone(worker, 0, stacks[i]) < 0) {
      printf("thread: clone failed\n");
      while (created-- > 0)
        join();
      exit(1);
    }
    created++;
  }

  for (i = 0; i < 3; i++) {
    if (join() < 0) {
      printf("thread: join failed\n");
      exit(1);
    }
  }

  printf("Final counter value: %d (Expected: 300000)\n", counter);
  exit(0);
}