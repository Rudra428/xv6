#include "types.h"
#include "stat.h"
#include "user.h"
#include "meminfo.h"

void
print_meminfo(char *label)
{
  struct meminfo mi;
  if(getmeminfo(&mi) < 0) {
    printf(2, "getmeminfo failed\n");
    return;
  }
  printf(1, "--- %s ---\n", label);
  printf(1, "Total System Pages : %d\n", mi.total_pages);
  printf(1, "Free System Pages  : %d\n", mi.free_pages);
  printf(1, "Process VMA Pages  : %d\n", mi.proc_pages);
  printf(1, "Page Table Pages   : %d\n\n", mi.pt_pages);
}

int
main(void)
{
  print_meminfo("Initial State");

  printf(1, "[*] Allocating 10 pages using sbrk...\n\n");
  sbrk(4096 * 10);
  
  print_meminfo("After sbrk(4096 * 10)");

  printf(1, "[*] Forking child process...\n\n");
  int pid = fork();
  
  if(pid < 0){
    printf(2, "fork failed\n");
    exit();
  }

  if(pid == 0){
    print_meminfo("Inside Child Process");
    exit();
  } else {
    wait(); // Wait for child to exit and be reaped by the kernel
    print_meminfo("After Child Exits & Reaped");
  }

  exit();
}