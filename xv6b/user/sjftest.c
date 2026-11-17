#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void work(long n) {
  volatile long c = 0;
  for (long i = 0; i < n; i++) c++;
}

int main(void) {
  int bt[]    = {100, 40, 10};
  long load[] = {900000000L, 450000000L, 150000000L};
  int delay[] = {15, 10, 0};

  printf("Starting SRTF Test...\n");

  for (int i = 0; i < 3; i++) {
    if (fork() == 0) {
      setbt(getpid(), bt[i]);
      pause(2);
      printf("P%d (burst %d) started\n", i + 1, bt[i]);
      work(load[i]);
      printf("P%d (burst %d) finished\n", i + 1, bt[i]);
      exit(0);
    }
    if (delay[i] > 0) pause(delay[i]);
  }

  for (int i = 0; i < 3; i++) wait(0);
  printf("SRTF Test Done!\n");
  exit(0);
}