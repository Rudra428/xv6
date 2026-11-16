#include "types.h"
#include "user.h"

#define NREFS 20
#define MAXFRAMES 5

static int refs[NREFS] = {
  7, 0, 1, 2, 0, 3, 0, 4, 2, 3,
  0, 3, 2, 1, 2, 0, 1, 7, 0, 1
};

static char *names[] = {"FIFO", "LRU", "Optimal"};

static void
simulate(int algorithm, int nframes, int verbose, int *faults, int *hits)
{
  int frames[MAXFRAMES];
  int lastused[MAXFRAMES];
  int i, j, k, victim, next, hit, nextuse, farthest;

  *faults = 0;
  *hits = 0;
  next = 0;
  for(j = 0; j < nframes; j++){
    frames[j] = -1;
    lastused[j] = -1;
  }

  for(i = 0; i < NREFS; i++){
    hit = 0;
    victim = -1;

    for(j = 0; j < nframes; j++){
      if(frames[j] == refs[i]){
        hit = 1;
        victim = j;
        break;
      }
      if(frames[j] == -1 && victim == -1)
        victim = j;
    }

    if(hit){
      (*hits)++;
    } else {
      (*faults)++;

      if(victim == -1){
        if(algorithm == 0){
          victim = next;
          next = (next + 1) % nframes;
        } else if(algorithm == 1){
          victim = 0;
          for(j = 1; j < nframes; j++)
            if(lastused[j] < lastused[victim])
              victim = j;
        } else {
          victim = 0;
          farthest = -1;
          for(j = 0; j < nframes; j++){
            nextuse = NREFS;
            for(k = i + 1; k < NREFS; k++){
              if(refs[k] == frames[j]){
                nextuse = k;
                break;
              }
            }
            if(nextuse > farthest){
              farthest = nextuse;
              victim = j;
            }
          }
        }
      }
      frames[victim] = refs[i];
    }
    lastused[victim] = i;

    if(verbose){
      printf(1, "%d: %s [", refs[i], hit ? "hit" : "fault");
      for(j = 0; j < nframes; j++){
        if(j)
          printf(1, " ");
        if(frames[j] == -1)
          printf(1, "-");
        else
          printf(1, "%d", frames[j]);
      }
      printf(1, "]\n");
    }
  }
}

int
main(void)
{
  int frames, algorithm, faults, hits, verbose;

  printf(1, "Reference string: 7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1\n");
  printf(1, "Frame count | Algorithm | Faults | Hits | Hit ratio | Fault ratio\n");

  for(frames = 3; frames <= MAXFRAMES; frames++){
    for(algorithm = 0; algorithm < 3; algorithm++){
      verbose = (frames == 3);
      if(verbose)
        printf(1, "\n%s, %d frames:\n", names[algorithm], frames);
      simulate(algorithm, frames, verbose, &faults, &hits);
      printf(1, "%d | %s | %d | %d | %d%% | %d%%\n",
             frames, names[algorithm], faults, hits,
             hits * 100 / NREFS, faults * 100 / NREFS);
    }
  }

  printf(1, "\nWith 3 frames, Optimal has the fewest faults. "
            "With 4 and 5 frames, Optimal ties LRU.\n");
  exit();
}
