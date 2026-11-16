#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define LOCK1 0
#define LOCK2 1

int pipes[10][2]; // Up to 10 semaphores

void sem_init(int id, int val) {
    pipe(pipes[id]);
    for (int i = 0; i < val; i++)
        write(pipes[id][1], "x", 1);
}
void sem_wait(int id) {
    char c;
    read(pipes[id][0], &c, 1);
}
void sem_post(int id) {
    write(pipes[id][1], "x", 1);
}

void run_test(int ordered) {
    sem_init(LOCK1, 1);
    sem_init(LOCK2, 1);

    int pidA = fork();
    if (pidA == 0) {
        // Process A: Always acquires Lock1 -> Lock2
        printf("[t=%d] Process A trying Lock1\n", uptime());
        sem_wait(LOCK1);
        printf("[t=%d] Process A acquired Lock1\n", uptime());

        pause(10); // Deliberate delay so B grabs its first lock

        printf("[t=%d] Process A waiting for Lock2\n", uptime());
        sem_wait(LOCK2);
        printf("[t=%d] Process A acquired Lock2 -> Working & Releasing!\n", uptime());

        sem_post(LOCK2);
        sem_post(LOCK1);
        exit(0);
    }

    int pidB = fork();
    if (pidB == 0) {
        if (!ordered) {
            // BAD ORDER (Lock2 -> Lock1): Causes Circular Wait!
            printf("[t=%d] Process B trying Lock2\n", uptime());
            sem_wait(LOCK2);
            printf("[t=%d] Process B acquired Lock2\n", uptime());

            pause(10);

            printf("[t=%d] Process B waiting for Lock1\n", uptime());
            sem_wait(LOCK1);
            printf("[t=%d] Process B acquired Lock1 -> Working & Releasing!\n", uptime());

            sem_post(LOCK1);
            sem_post(LOCK2);
        } else {
            // FIXED GLOBAL ORDER (Lock1 -> Lock2): Prevents Deadlock!
            printf("[t=%d] Process B trying Lock1\n", uptime());
            sem_wait(LOCK1);
            printf("[t=%d] Process B acquired Lock1\n", uptime());

            pause(10);

            printf("[t=%d] Process B waiting for Lock2\n", uptime());
            sem_wait(LOCK2);
            printf("[t=%d] Process B acquired Lock2 -> Working & Releasing!\n", uptime());

            sem_post(LOCK2);
            sem_post(LOCK1);
        }
        exit(0);
    }

    if (!ordered) {
        pause(35); // Wait 35 ticks; A and B will be stuck deadlocked!
        printf("[t=%d] TIMEOUT: Process A and Process B are DEADLOCKED! Killing both...\n", uptime());
        kill(pidA);
        kill(pidB);
    }

    wait(0);
    wait(0);
}

int main() {
    printf("=== PART 1: Unordered Acquisition (Deadlock Demo) ===\n");
    run_test(0);

    printf("\n=== PART 2: Global Resource Ordering (Lock1 -> Lock2) ===\n");
    run_test(1);
    printf("[t=%d] SUCCESS: Both processes completed without deadlock!\n", uptime());

    exit(0);
}