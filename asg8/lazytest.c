#include "types.h"
#include "user.h"
#include "meminfo.h"

#define REGION_SIZE (32 * 1024 * 1024)

static void
print_meminfo(char *label)
{
  struct meminfo mi;

  if(getmeminfo(&mi) < 0){
    printf(2, "getmeminfo failed\n");
    return;
  }
  printf(1, "%s: free=%d process=%d page-table=%d pages\n",
         label, mi.free_pages, mi.proc_pages, mi.pt_pages);
}

int
main(void)
{
  int pid;
  volatile char *region;

  print_meminfo("Before sbrk");
  region = (volatile char*)sbrk(REGION_SIZE);
  if(region == (char*)-1){
    printf(2, "large sbrk failed\n");
    exit();
  }
  print_meminfo("After reserving 32 MB");

  region[0] = 1;
  region[8 * 1024 * 1024] = 2;
  region[16 * 1024 * 1024] = 3;
  region[31 * 1024 * 1024] = 4;
  print_meminfo("After touching four pages");

  printf(1, "Writing one byte from an untouched lazy page: [");
  write(1, (const void*)(region + 24 * 1024 * 1024), 1);
  printf(1, "]\n");
  print_meminfo("After system call touches a lazy buffer");

  pid = fork();
  if(pid < 0){
    printf(2, "fork failed\n");
    exit();
  }
  if(pid == 0){
    region[12 * 1024 * 1024] = 5;
    print_meminfo("Child after touching an inherited lazy page");
    exit();
  }
  wait();
  print_meminfo("Parent after child exit and reap");

  pid = fork();
  if(pid < 0){
    printf(2, "fork for invalid-access test failed\n");
    exit();
  }
  if(pid == 0){
    region[REGION_SIZE] = 1;
    printf(2, "ERROR: access beyond the heap was allowed\n");
    exit();
  }
  wait();
  printf(1, "Invalid-access child was reaped; parent remains alive\n");
  exit();
}
