#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  printf(1, "--- Initial Page Table ---\n");
  vmprint();

  printf(1, "\n--- Extending heap by 4 pages (sbrk) ---\n");
  // 4096 bytes per page * 4 pages = 16384 bytes
  if((int)sbrk(4 * 4096) < 0) {
    printf(1, "sbrk failed\n");
    exit();
  }

  printf(1, "\n--- Updated Page Table ---\n");
  vmprint();

  exit();
}