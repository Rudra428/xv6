#include "types.h"
#include "user.h"

int ref[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
int n = 20;

// is_lru = 0 for FIFO, 1 for LRU
int simulate(int is_lru, int num_frames, int print_trace) {
  int frames[5], time[5];
  int faults = 0, timer = 0;

  // Initialize empty frames
  for(int i = 0; i < num_frames; i++) { frames[i] = -1; time[i] = 0; }

  for(int i = 0; i < n; i++) {
    int page = ref[i];
    int hit = 0;
    timer++;
    
    // Check for hit
    for(int j = 0; j < num_frames; j++) {
      if(frames[j] == page) {
        hit = 1;
        if(is_lru) time[j] = timer; // LRU updates time on hit!
        break;
      }
    }

    // Page Fault
    if(!hit) {
      faults++;
      int victim = 0;
      // Find the frame with the smallest time (oldest)
      for(int j = 1; j < num_frames; j++) {
        if(time[j] < time[victim]) victim = j;
      }
      frames[victim] = page;
      time[victim] = timer; // Both FIFO and LRU update time on insertion
    }

    // Print step if requested
    if(print_trace) {
      printf(1, "%d -> [ ", page);
      for(int j = 0; j < num_frames; j++) {
        if(frames[j] == -1) printf(1, "- ");
        else printf(1, "%d ", frames[j]);
      }
      printf(1, "] %s\n", hit ? "HIT" : "FAULT");
    }
  }
  return faults;
}

int main() {
  printf(1, "--- FIFO (3 Frames) ---\n");
  int f_faults = simulate(0, 3, 1);
  printf(1, "\nTotal FIFO Faults: %d\n\n", f_faults);

  printf(1, "--- LRU (3 Frames) ---\n");
  int l_faults = simulate(1, 3, 1);
  printf(1, "\nTotal LRU Faults: %d\n\n", l_faults);

  printf(1, "--- Summary Table ---\n");
  printf(1, "Frames | FIFO Faults | LRU Faults | Hit Ratio(FIFO) | Hit Ratio(LRU)\n");
  
  for(int f = 3; f <= 5; f++) {
    int fifo_f = simulate(0, f, 0);
    int lru_f = simulate(1, f, 0);
    // (20 - faults) / 20 * 100 simplifies to (20 - faults) * 5 for integer math
    printf(1, "   %d   |      %d     |     %d     |       %d%%       |       %d%%\n", 
           f, fifo_f, lru_f, (20 - fifo_f) * 5, (20 - lru_f) * 5);
  }

  exit();
}