#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

struct event {
  char marker;
  uint tick;
};

static void
work(int ticks)
{
  volatile uint64 count = 0;
  uint start = uptime();

  while (uptime() - start < (uint)ticks)
    count++;
}

static int
record_completion(int fd, char marker)
{
  struct event event;

  event.marker = marker;
  event.tick = uptime();
  return write(fd, &event, sizeof(event)) == (int)sizeof(event) ? 0 : -1;
}

static int
spawn(int fd, int burst, char marker, uint *arrival)
{
  *arrival = uptime();
  int pid = fork();

  if (pid < 0)
    return -1;
  if (pid == 0) {
    work(burst);
    if (record_completion(fd, marker) < 0)
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
  uint arrivals[4];
  struct event events[4];
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
    if (spawn(fd[1], bursts[i], markers[i], &arrivals[i]) < 0) {
      printf("hrrntest: fork or set_burst failed\n");
      exit(1);
    }
  }
  close(fd[1]);

  for (int i = 0; i < 4; i++) {
    if (read(fd[0], &events[i], sizeof(events[i])) != (int)sizeof(events[i])) {
      printf("hrrntest: failed to read completion order\n");
      exit(1);
    }
    order[i] = events[i].marker;
  }
  close(fd[0]);

  for (int i = 0; i < 4; i++)
    printf("hrrntest: process %c arrived at tick %d\n",
           markers[i], arrivals[i]);
  for (int i = 0; i < 4; i++)
    printf("hrrntest: process %c completed at tick %d\n",
           events[i].marker, events[i].tick);
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
