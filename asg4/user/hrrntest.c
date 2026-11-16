#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

static void
work(int ticks)
{
  volatile uint64 count = 0;
  uint start = uptime();

  while (uptime() - start < (uint)ticks)
    count++;
}

static int
spawn(int fd, int burst, char marker)
{
  int pid = fork();

  if (pid < 0)
    return -1;
  if (pid == 0) {
    work(burst);
    if (write(fd, &marker, 1) != 1)
      exit(1);
    exit(0);
  }
  if (set_burst(pid, burst) < 0) {
    kill(pid);
    wait(0);
    return -1;
  }
  return pid;
}

int
main(void)
{
  int fd[2];
  int bursts[] = {2, 3, 40, 60};
  char markers[] = {'s', 't', 'L', 'M'};
  char order[4];

  if (set_scheduler(SCHED_HRRN) < 0) {
    printf("hrrntest: could not select HRRN\n");
    exit(1);
  }
  printf("hrrntest: mixed CPU bursts\n");
  if (pipe(fd) < 0) {
    printf("hrrntest: pipe failed\n");
    exit(1);
  }

  for (int i = 0; i < 4; i++) {
    if (spawn(fd[1], bursts[i], markers[i]) < 0) {
      printf("hrrntest: fork or set_burst failed\n");
      exit(1);
    }
  }
  close(fd[1]);

  for (int i = 0; i < 4; i++) {
    if (read(fd[0], &order[i], 1) != 1) {
      printf("hrrntest: failed to read completion order\n");
      exit(1);
    }
  }
  close(fd[0]);

  printf("hrrntest: completion order %c%c%c%c\n",
         order[0], order[1], order[2], order[3]);
  for (int i = 0; i < 4; i++)
    wait(0);

  if (order[0] != 's' || order[1] != 't' ||
      (order[2] != 'L' && order[2] != 'M') ||
      (order[3] != 'L' && order[3] != 'M')) {
    printf("hrrntest: unexpected HRRN completion order\n");
    exit(1);
  }
  printf("hrrntest: short jobs ran first; long jobs were not starved\n");
  exit(0);
}
