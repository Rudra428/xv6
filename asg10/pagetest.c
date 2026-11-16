#include "types.h"
#include "user.h"

int
main(void)
{
  char *page;
  int pid;

  page = sbrk(4096);
  if(page == (char*)-1){
    printf(1, "pagetest: sbrk failed\n");
    exit();
  }
  page[0] = 'a';
  if(pageinfo((int)page) < 0){
    printf(1, "pagetest: pageinfo failed\n");
    exit();
  }
  if(setreadonly((int)page) < 0){
    printf(1, "pagetest: setreadonly failed\n");
    sbrk(-4096);
    exit();
  }
  printf(1, "pagetest: page is now read-only; child will try to write\n");
  pid = fork();
  if(pid < 0){
    printf(1, "pagetest: fork failed\n");
    sbrk(-4096);
    exit();
  }
  if(pid == 0){
    page[0] = 'b';
    printf(1, "pagetest: unexpected write success\n");
    exit();
  }
  wait();
  printf(1, "pagetest: child terminated after the protected write fault\n");
  sbrk(-4096);
  exit();
}
