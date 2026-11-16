#include "types.h"
#include "user.h"

static void
dump(char *label)
{
  uint pages = (uint)sbrk(0) / 4096;

  printf(1, "%s: process pages=%d, expected user-accessible pages=%d "
         "(one guard page)\n", label, pages, pages - 1);
  vmprint();
}

int
main(void)
{
  int i;
  volatile char *heap;

  printf(1, "Page table at startup (guard page has U=0):\n");
  dump("Startup");

  heap = sbrk(8 * 4096);
  if(heap == (char*)-1){
    printf(2, "sbrk growth failed\n");
    exit();
  }
  for(i = 0; i < 8; i++)
    heap[i * 4096] = i;
  printf(1, "\nAfter growing and touching eight heap pages:\n");
  dump("After growth");

  if(sbrk(-4096) == (char*)-1){
    printf(2, "sbrk shrink failed\n");
    exit();
  }
  printf(1, "\nAfter shrinking the heap by one page:\n");
  dump("After shrink");
  exit();
}
