#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NPROC 5
#define NRES 3
#define NSEM (NRES + 1)
#define NEEDED 2
#define CYCLES 3
#define LOG_LOCK NRES

enum {
  PRINTER,
  SCANNER,
  DISK
};

static int semaphores[NSEM][2];
static char *resource_names[NRES] = {"printer", "scanner", "disk"};
static int capacity[NRES] = {2, 1, 2};
static int requirements[NPROC][NEEDED] = {
  {PRINTER, SCANNER},
  {PRINTER, DISK},
  {SCANNER, DISK},
  {PRINTER, SCANNER},
  {PRINTER, DISK},
};

static int
sem_init(int resource, int instances)
{
  char token = 'x';

  if (pipe(semaphores[resource]) < 0)
    return -1;
  for (int i = 0; i < instances; i++) {
    if (write(semaphores[resource][1], &token, 1) != 1) {
      close(semaphores[resource][0]);
      close(semaphores[resource][1]);
      return -1;
    }
  }
  return 0;
}

static int
sem_wait(int resource)
{
  char token;

  return read(semaphores[resource][0], &token, 1) == 1 ? 0 : -1;
}

static int
sem_post(int resource)
{
  char token = 'x';

  return write(semaphores[resource][1], &token, 1) == 1 ? 0 : -1;
}

#define LOG(...) do { \
  if (sem_wait(LOG_LOCK) < 0) { \
    printf("syncdeadlock: logging semaphore failure\n"); \
    exit(1); \
  } \
  printf(__VA_ARGS__); \
  if (sem_post(LOG_LOCK) < 0) { \
    printf("syncdeadlock: logging semaphore failure\n"); \
    exit(1); \
  } \
} while (0)

static void
work(int process)
{
  for (int cycle = 1; cycle <= CYCLES; cycle++) {
    int held = 0;

    for (int i = 0; i < NEEDED; i++) {
      int resource = requirements[process][i];

      LOG("[pid %d] cycle %d requests %s\n",
          getpid(), cycle, resource_names[resource]);
      if (sem_wait(resource) < 0) {
        LOG("[pid %d] failed waiting for %s\n",
            getpid(), resource_names[resource]);
        while (held > 0) {
          resource = requirements[process][--held];
          sem_post(resource);
        }
        exit(1);
      }
      held++;
      LOG("[pid %d] cycle %d granted %s\n",
          getpid(), cycle, resource_names[resource]);
    }

    LOG("[pid %d] cycle %d starts work\n", getpid(), cycle);
    pause(2);
    LOG("[pid %d] cycle %d releases resources\n", getpid(), cycle);
    while (held > 0) {
      int resource = requirements[process][--held];

      if (sem_post(resource) < 0) {
        LOG("[pid %d] failed releasing %s\n",
            getpid(), resource_names[resource]);
        exit(1);
      }
    }
  }
  LOG("[pid %d] completed all cycles\n", getpid());
  exit(0);
}

int
main(void)
{
  for (int resource = 0; resource < NRES; resource++) {
    if (sem_init(resource, capacity[resource]) < 0) {
      printf("syncdeadlock: could not initialize %s semaphore\n",
             resource_names[resource]);
      exit(1);
    }
  }
  if (sem_init(LOG_LOCK, 1) < 0) {
    printf("syncdeadlock: could not initialize logging semaphore\n");
    exit(1);
  }
  LOG("Resource capacities: printer=2, scanner=1, disk=2\n");
  LOG("Deadlock prevention: acquire resources in printer < scanner < disk order\n");

  for (int process = 0; process < NPROC; process++) {
    int pid = fork();

    if (pid < 0) {
      LOG("syncdeadlock: fork failed\n");
      for (int i = 0; i < process; i++)
        wait(0);
      exit(1);
    }
    if (pid == 0)
      work(process);
  }

  for (int process = 0; process < NPROC; process++) {
    int status;
    int pid = wait(&status);

    if (pid < 0 || status != 0) {
      LOG("syncdeadlock: child process failed\n");
      exit(1);
    }
  }

  LOG("All 5 processes completed %d cycles; no deadlock occurred.\n", CYCLES);
  for (int resource = 0; resource < NSEM; resource++) {
    close(semaphores[resource][0]);
    close(semaphores[resource][1]);
  }
  exit(0);
}
