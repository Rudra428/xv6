#include "types.h"
#include "user.h"

#define PGSIZE 4096

int
main(void)
{
  char *base;
  int i, va, pa;

  base = sbrk(3 * PGSIZE);
  if(base == (char*)-1){
    printf(1, "vttest: sbrk failed\n");
    exit();
  }
  printf(1, "vttest: allocated range starting at %x\n", base);
  for(i = 0; i < 3; i++){
    va = (int)(base + i * PGSIZE);
    pa = vtop(va);
    if(pa < 0)
      printf(1, "vttest: translation failed for %x\n", va);
    else
      printf(1, "vttest: virtual %x -> physical %x\n", va, pa);
  }
  va = (int)(base + 3 * PGSIZE);
  pa = vtop(va);
  printf(1, "vttest: unmapped virtual %x -> %d\n", va, pa);
  sbrk(-3 * PGSIZE);
  exit();
}
