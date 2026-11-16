#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(void)
{
  if (set_scheduler(SCHED_MLFQ) < 0) {
    printf("mlfqtest: could not select MLFQ\n");
    exit(1);
  }

  int cpu_pid = fork();
  if (cpu_pid < 0) {
    printf("mlfqtest: CPU-bound fork failed\n");
    exit(1);
  }
  if (cpu_pid == 0) {
    volatile uint64 count = 0;
    for (;;)
      count++;
  }

  int io_pid = fork();
  if (io_pid < 0) {
    kill(cpu_pid);
    wait(0);
    printf("mlfqtest: I/O-bound fork failed\n");
    exit(1);
  }
  if (io_pid == 0) {
    for (;;)
      pause(1);
  }

  pause(20);
  int cpu_level = get_queue_level(cpu_pid);
  int io_level = get_queue_level(io_pid);
  printf("mlfqtest: CPU-bound level %d, I/O-bound level %d\n",
         cpu_level, io_level);

  kill(cpu_pid);
  kill(io_pid);
  wait(0);
  wait(0);

  if (cpu_level != 2 || io_level != 0) {
    printf("mlfqtest: expected CPU-bound level 2 and I/O-bound level 0\n");
    exit(1);
  }
  printf("mlfqtest: CPU-bound job demoted; I/O-bound job kept high priority\n");
  exit(0);
}
