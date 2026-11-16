#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXP 4
#define MAXR 3

static void
print_state(int n, int m, int allocation[MAXP][MAXR],
            int matrix[MAXP][MAXR], int available[MAXR], int is_max)
{
  printf("Available:");
  for (int j = 0; j < m; j++)
    printf(" %d", available[j]);
  printf("\nAllocation / %s:\n", is_max ? "Need" : "Request");
  for (int i = 0; i < n; i++) {
    printf("P%d alloc", i);
    for (int j = 0; j < m; j++)
      printf(" %d", allocation[i][j]);
    printf(" %s", is_max ? "need" : "request");
    for (int j = 0; j < m; j++)
      printf(" %d", is_max ? matrix[i][j] - allocation[i][j] : matrix[i][j]);
    printf("\n");
  }
}

static int
detect_deadlock(int n, int m, int allocation[MAXP][MAXR],
                int request[MAXP][MAXR], int available[MAXR],
                int deadlocked[MAXP])
{
  int work[MAXR], finish[MAXP], count = 0;

  for (int j = 0; j < m; j++)
    work[j] = available[j];
  for (int i = 0; i < n; i++) {
    int holds_resource = 0;
    for (int j = 0; j < m; j++)
      holds_resource |= allocation[i][j] != 0;
    finish[i] = !holds_resource;
  }

  printf("Detection steps:");
  for (;;) {
    int found = 0;
    for (int i = 0; i < n; i++) {
      if (finish[i])
        continue;
      int can_finish = 1;
      for (int j = 0; j < m; j++)
        if (request[i][j] > work[j])
          can_finish = 0;
      if (!can_finish)
        continue;
      finish[i] = 1;
      found = 1;
      printf(" P%d", i);
      for (int j = 0; j < m; j++)
        work[j] += allocation[i][j];
      printf(" (Available");
      for (int j = 0; j < m; j++)
        printf(" %d", work[j]);
      printf(")");
    }
    if (!found)
      break;
  }
  printf("\n");

  for (int i = 0; i < n; i++) {
    if (!finish[i])
      deadlocked[count++] = i;
  }
  return count;
}

static int
is_safe_state(int n, int m, int allocation[MAXP][MAXR],
              int maximum[MAXP][MAXR], int available[MAXR], int sequence[MAXP])
{
  int work[MAXR], finish[MAXP];

  for (int j = 0; j < m; j++)
    work[j] = available[j];
  for (int i = 0; i < n; i++) {
    finish[i] = 0;
    for (int j = 0; j < m; j++)
      if (maximum[i][j] < allocation[i][j])
        return 0;
  }

  int completed = 0;
  for (;;) {
    int found = 0;
    for (int i = 0; i < n; i++) {
      if (finish[i])
        continue;
      int can_finish = 1;
      for (int j = 0; j < m; j++)
        if (maximum[i][j] - allocation[i][j] > work[j])
          can_finish = 0;
      if (!can_finish)
        continue;
      finish[i] = 1;
      sequence[completed++] = i;
      found = 1;
      for (int j = 0; j < m; j++)
        work[j] += allocation[i][j];
      printf("  P%d finishes; Available now:", i);
      for (int j = 0; j < m; j++)
        printf(" %d", work[j]);
      printf("\n");
      print_state(n, m, allocation, maximum, work, 1);
    }
    if (!found)
      break;
  }
  return completed == n;
}

static void
pipe_deadlock_demo(void)
{
  int pipe1[2], pipe2[2];
  int child_a, child_b, deadlocked[MAXP];
  int allocation[MAXP][MAXR] = {{1, 0}, {0, 1}};
  int request[MAXP][MAXR] = {{0, 1}, {1, 0}};
  int available[MAXR] = {0, 0};

  if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
    printf("pipe deadlock demo: pipe creation failed\n");
    return;
  }
  child_a = fork();
  if (child_a == 0) {
    char ch;
    close(pipe1[0]);
    close(pipe2[1]);
    read(pipe2[0], &ch, 1);
    exit(0);
  }
  if (child_a < 0) {
    printf("pipe deadlock demo: fork failed\n");
    exit(1);
  }
  child_b = fork();
  if (child_b == 0) {
    char ch;
    close(pipe2[0]);
    close(pipe1[1]);
    read(pipe1[0], &ch, 1);
    exit(0);
  }
  if (child_b < 0) {
    kill(child_a);
    wait(0);
    printf("pipe deadlock demo: fork failed\n");
    exit(1);
  }

  close(pipe1[0]);
  close(pipe1[1]);
  close(pipe2[0]);
  close(pipe2[1]);
  pause(10);
  printf("Pipe cycle: A holds Pipe1/waits Pipe2; B holds Pipe2/waits Pipe1\n");
  print_state(2, 2, allocation, request, available, 0);
  int count = detect_deadlock(2, 2, allocation, request, available,
                              deadlocked);
  printf("Detected blocked pipe processes:");
  for (int i = 0; i < count; i++)
    printf(" P%d", deadlocked[i]);
  printf("\n");

  kill(child_a);
  kill(child_b);
  wait(0);
  wait(0);
}

int
main(int argc, char *argv[])
{
  int deadlocked[MAXP], sequence[MAXP];

  {
    int allocation[MAXP][MAXR] = {{1, 0}, {0, 1}};
    int request[MAXP][MAXR] = {{0, 1}, {1, 0}};
    int available[MAXR] = {0, 0};
    print_state(2, 2, allocation, request, available, 0);
    int count = detect_deadlock(2, 2, allocation, request, available,
                                deadlocked);
    printf("deadlocked matrix: %d process(es)", count);
    for (int i = 0; i < count; i++)
      printf(" P%d", deadlocked[i]);
    printf("\n");
  }

  {
    int allocation[MAXP][MAXR] = {{1, 0}, {0, 1}};
    int request[MAXP][MAXR] = {{0, 1}, {0, 0}};
    int available[MAXR] = {0, 0};
    print_state(2, 2, allocation, request, available, 0);
    int count = detect_deadlock(2, 2, allocation, request, available,
                                deadlocked);
    printf("safe detection matrix: %d deadlocked process(es)\n", count);
  }

  {
    int allocation[MAXP][MAXR] = {{1, 0}, {0, 1}};
    int maximum[MAXP][MAXR] = {{2, 2}, {2, 1}};
    int available[MAXR] = {2, 1};
    int request[MAXP][MAXR] = {{1, 0}, {0, 0}};
    int n = 2, m = 2;

    print_state(n, m, allocation, maximum, available, 1);
    printf("Initial Banker state: %s\n",
           is_safe_state(n, m, allocation, maximum, available, sequence)
               ? "safe"
               : "unsafe");
    printf("Request P0: 1 0\n");
    for (int j = 0; j < m; j++) {
      if (request[0][j] > maximum[0][j] - allocation[0][j] ||
          request[0][j] > available[j]) {
        printf("Request is invalid or unavailable\n");
        exit(1);
      }
      available[j] -= request[0][j];
      allocation[0][j] += request[0][j];
    }
    print_state(n, m, allocation, maximum, available, 1);
    printf("After request: %s\n",
           is_safe_state(n, m, allocation, maximum, available, sequence)
               ? "safe"
               : "unsafe");
  }

  pipe_deadlock_demo();
  exit(0);
}
