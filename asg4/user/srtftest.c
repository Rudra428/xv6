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
record_completion(int fd, char marker, uint tick)
{
  struct event event;

  event.marker = marker;
  event.tick = tick;
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
    uint started = uptime();
    printf("srtftest: process %c started at tick %d\n", marker, started);
    work(burst);
    uint completed = uptime();
    printf("srtftest: process %c completed at tick %d\n", marker, completed);
    if (record_completion(fd, marker, completed) < 0)
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
  uint arrivals[3];
  uint parent_start;
  struct event events[4];
  char order[4];

  if (set_scheduler(SCHED_SRTF) < 0) {
    printf("srtftest: could not select SRTF\n");
    exit(1);
  }
  printf("srtftest: staggered CPU bursts\n");
  if (pipe(fd) < 0 || set_burst(getpid(), 30) < 0) {
    printf("srtftest: setup failed\n");
    exit(1);
  }

  pids[0] = spawn(fd[1], 60, 'A', &arrivals[0]);
  if (pids[0] < 0)
    exit(1);
  printf("srtftest: process A arrived at tick %d\n", arrivals[0]);
  pause(1);

  pids[1] = spawn(fd[1], 50, 'B', &arrivals[1]);
  if (pids[1] < 0) {
    kill(pids[0]);
    wait(0);
    exit(1);
  }
  printf("srtftest: process B arrived at tick %d\n", arrivals[1]);
  pause(1);

  pids[2] = spawn(fd[1], 4, 'S', &arrivals[2]);
  if (pids[2] < 0) {
    kill(pids[0]);
    kill(pids[1]);
    wait(0);
    wait(0);
    exit(1);
  }
  printf("srtftest: process S arrived at tick %d\n", arrivals[2]);

  parent_start = uptime();
  printf("srtftest: process P started at tick %d\n", parent_start);
  work(25);
  uint parent_completed = uptime();
  printf("srtftest: process P completed at tick %d\n", parent_completed);
  if (record_completion(fd[1], 'P', parent_completed) < 0)
    exit(1);
  close(fd[1]);

  for (int i = 0; i < 4; i++) {
    if (read(fd[0], &events[i], sizeof(events[i])) != (int)sizeof(events[i])) {
      printf("srtftest: failed to read completion order\n");
      exit(1);
    }
    order[i] = events[i].marker;
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
