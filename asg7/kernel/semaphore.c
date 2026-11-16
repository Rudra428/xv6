#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define NSEMAPHORE 10

struct semaphore {
  int value;
  int initialized;
  struct spinlock lock;
};

static struct semaphore semaphores[NSEMAPHORE];
static struct spinlock semaphores_lock;

void
seminit(void)
{
  initlock(&semaphores_lock, "semaphores");
  for (int i = 0; i < NSEMAPHORE; i++) {
    initlock(&semaphores[i].lock, "semaphore");
    semaphores[i].value = 0;
    semaphores[i].initialized = 0;
  }
}

int
sem_init(int sem_id, int value)
{
  if (sem_id < 0 || sem_id >= NSEMAPHORE || value < 0)
    return -1;

  acquire(&semaphores_lock);
  struct semaphore *sem = &semaphores[sem_id];
  acquire(&sem->lock);
  sem->value = value;
  sem->initialized = 1;
  wakeup(sem);
  release(&sem->lock);
  release(&semaphores_lock);
  return 0;
}

int
sem_wait(int sem_id)
{
  if (sem_id < 0 || sem_id >= NSEMAPHORE)
    return -1;

  acquire(&semaphores_lock);
  struct semaphore *sem = &semaphores[sem_id];
  if (!sem->initialized) {
    release(&semaphores_lock);
    return -1;
  }
  acquire(&sem->lock);
  release(&semaphores_lock);

  while (sem->value == 0) {
    if (killed(myproc())) {
      release(&sem->lock);
      return -1;
    }
    sleep_prepare(sem);
    release(&sem->lock);
    sleep();
    acquire(&sem->lock);
  }
  sem->value--;
  release(&sem->lock);
  return 0;
}

int
sem_signal(int sem_id)
{
  if (sem_id < 0 || sem_id >= NSEMAPHORE)
    return -1;

  acquire(&semaphores_lock);
  struct semaphore *sem = &semaphores[sem_id];
  if (!sem->initialized) {
    release(&semaphores_lock);
    return -1;
  }
  acquire(&sem->lock);
  release(&semaphores_lock);
  if (sem->value == 0x7fffffff) {
    release(&sem->lock);
    return -1;
  }
  sem->value++;
  wakeup(sem);
  release(&sem->lock);
  return 0;
}
