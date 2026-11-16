#include "types.h"
#include "user.h"

int
main(void)
{
  char *p;

  printf(1, "memtest: before allocation\n");
  memstats();
  p = sbrk(4096);
  if(p == (char*)-1){
    printf(1, "memtest: sbrk failed\n");
    exit();
  }
  printf(1, "memtest: allocated page at %x\n", p);
  memstats();
  if(sbrk(-4096) == (char*)-1){
    printf(1, "memtest: failed to release page\n");
    exit();
  }
  printf(1, "memtest: after release\n");
  memstats();
  exit();
}
