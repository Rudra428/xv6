#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_memstats(void)
{
  uint allocated, freed, current, available;

  getmemstats(&allocated, &freed, &current, &available);
  cprintf("memstats: allocated %d freed %d current %d available %d pages\n",
          allocated, freed, current, available);
  return 0;
}

int
sys_vtop(void)
{
  int va;

  if(argint(0, &va) < 0 || va < 0)
    return -1;
  return vm_vtop((uint)va);
}

int
sys_pageinfo(void)
{
  int va;
  uint pa, flags;

  if(argint(0, &va) < 0 || va < 0 ||
     vm_pageinfo((uint)va, &pa, &flags) < 0)
    return -1;
  cprintf("pageinfo: va %x frame %x flags P=%d W=%d U=%d\n",
          va, pa >> 12, (flags & PTE_P) != 0,
          (flags & PTE_W) != 0, (flags & PTE_U) != 0);
  return 0;
}

int
sys_setreadonly(void)
{
  int va;

  if(argint(0, &va) < 0 || va < 0)
    return -1;
  return vm_setreadonly((uint)va);
}
