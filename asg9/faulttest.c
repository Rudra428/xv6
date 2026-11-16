#include "types.h"
#include "user.h"

int main() {
  char *mem = sbrk(32 * 4096);
  
  // Write to exactly 5 pages
  for(int i = 0; i < 5; i++) {
    mem[i * 4096] = 'A';
  }
  printf(1, "Handled faults: %d\n", getfaults()); // MUST PRINT 5
  
  // Test Out of Bounds
  if(fork() == 0) {
    mem[33 * 4096] = 'B'; // Should kill child
    printf(1, "FAIL: Child survived\n");
  } else {
    wait();
    printf(1, "Child killed successfully.\n");
  }
  
  exit();
}