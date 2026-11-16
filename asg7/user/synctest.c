#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define SEM_COUNTER 0
#define SEM_EMPTY 1
#define SEM_FULL 2
#define SEM_BUFFER 3
#define BUFFER_SIZE 5
#define ITEM_COUNT 10

volatile int counter;
int buffer[BUFFER_SIZE];
int put_index;
int get_index;
char stacks[5][4096] __attribute__((aligned(4096)));

void
counter_worker(void *arg)
{
  for (int i = 0; i < 100000; i++) {
    if (sem_wait(SEM_COUNTER) < 0)
      exit(1);
    counter++;
    if (sem_signal(SEM_COUNTER) < 0)
      exit(1);
  }
  exit(0);
}

void
producer(void *arg)
{
  for (int item = 1; item <= ITEM_COUNT; item++) {
    if (sem_wait(SEM_EMPTY) < 0 || sem_wait(SEM_BUFFER) < 0)
      exit(1);
    buffer[put_index] = item;
    put_index = (put_index + 1) % BUFFER_SIZE;
    printf("produced %d\n", item);
    if (sem_signal(SEM_BUFFER) < 0 || sem_signal(SEM_FULL) < 0)
      exit(1);
  }
  exit(0);
}

void
consumer(void *arg)
{
  for (int i = 0; i < ITEM_COUNT; i++) {
    int item;
    if (sem_wait(SEM_FULL) < 0 || sem_wait(SEM_BUFFER) < 0)
      exit(1);
    item = buffer[get_index];
    get_index = (get_index + 1) % BUFFER_SIZE;
    printf("consumed %d\n", item);
    if (sem_signal(SEM_BUFFER) < 0 || sem_signal(SEM_EMPTY) < 0)
      exit(1);
  }
  exit(0);
}

int
main(int argc, char *argv[])
{
  if (sem_init(SEM_COUNTER, 1) < 0 ||
      sem_init(SEM_EMPTY, BUFFER_SIZE) < 0 ||
      sem_init(SEM_FULL, 0) < 0 ||
      sem_init(SEM_BUFFER, 1) < 0) {
    printf("synctest: sem_init failed\n");
    exit(1);
  }

  counter = 0;
  int created = 0;
  for (int i = 0; i < 3; i++) {
    if (clone(counter_worker, 0, stacks[i]) < 0) {
      printf("synctest: counter clone failed\n");
      while (created-- > 0)
        join();
      exit(1);
    }
    created++;
  }
  for (int i = 0; i < 3; i++) {
    if (join() < 0) {
      printf("synctest: counter join failed\n");
      exit(1);
    }
  }
  printf("synchronized counter: %d (expected 300000)\n", counter);

  int producer_pid = clone(producer, 0, stacks[3]);
  if (producer_pid < 0) {
    printf("synctest: producer/consumer clone failed\n");
    exit(1);
  }
  if (clone(consumer, 0, stacks[4]) < 0) {
    kill(producer_pid);
    join();
    printf("synctest: producer/consumer clone failed\n");
    exit(1);
  }
  if (join() < 0 || join() < 0) {
    printf("synctest: producer/consumer join failed\n");
    exit(1);
  }
  exit(0);
}
