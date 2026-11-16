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
  int pids[3];
  char order[4];
  char marker = 'P';

  if (set_scheduler(SCHED_SRTF) < 0) {
    printf("srtftest: could not select SRTF\n");
    exit(1);
  }
  printf("srtftest: staggered CPU bursts\n");
  if (pipe(fd) < 0 || set_burst(getpid(), 30) < 0) {
    printf("srtftest: setup failed\n");
    exit(1);
  }

  pids[0] = spawn(fd[1], 60, 'A');
  if (pids[0] < 0)
    exit(1);
  pause(1);

  pids[1] = spawn(fd[1], 50, 'B');
  if (pids[1] < 0) {
    kill(pids[0]);
    wait(0);
    exit(1);
  }
  pause(1);

  pids[2] = spawn(fd[1], 4, 'S');
  if (pids[2] < 0) {
    kill(pids[0]);
    kill(pids[1]);
    wait(0);
    wait(0);
    exit(1);
  }

  work(25);
  if (write(fd[1], &marker, 1) != 1)
    exit(1);
  close(fd[1]);

  for (int i = 0; i < 4; i++) {
    if (read(fd[0], &order[i], 1) != 1) {
      printf("srtftest: failed to read completion order\n");
      exit(1);
    }
  }
  close(fd[0]);

  printf("srtftest: completion order %c%c%c%c\n",
         order[0], order[1], order[2], order[3]);
  for (int i = 0; i < 3; i++)
    wait(0);

  if (order[0] != 'S' || order[1] != 'P') {
    printf("srtftest: short job did not preempt the running parent job\n");
    exit(1);
  }
  printf("srtftest: short job preempted the running parent job\n");
  exit(0);
}
