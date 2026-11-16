#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct shared{
    int buffer[5];
    int in,out;
};
#define mutex 0
#define empty 1
#define full 2
int main(){
    struct shared *shm = (struct shared *)shm_get();
    shm->in=shm->out=0;
    sem_init(mutex,1);
    sem_init(empty,5);
    sem_init(full,0);
    int pid = fork();
    
    if(pid < 0) {
        printf("Fork failed!\n");
        exit(1);
    }
    if(pid==0){
        shm_get();
        for(int i = 0; i < 20; i++) {
            sem_wait(full);   // Blocks if the buffer is completely empty
            sem_wait(mutex);  // Lock buffer
            
            int item = shm->buffer[shm->out];
            printf("Consumer read: %d (from index %d)\n", item, shm->out);
            shm->out = (shm->out + 1) % 5;
            
            sem_signal(mutex);  // Unlock buffer
            sem_signal(empty);  // Signal that an empty slot opened up
            
            pause(5); // Consumer runs slightly slower than producer
        }
        exit(0);
    }
    else{
        for(int i = 1; i <= 20; i++) {
            sem_wait(empty);  // Blocks if the buffer is completely full
            sem_wait(mutex);  // Lock buffer
            
            shm->buffer[shm->in] = i;
            printf("Producer wrote: %d (at index %d)\n", i, shm->in);
            shm->in = (shm->in + 1) % 5;
            
            sem_signal(mutex);  // Unlock buffer
            sem_signal(full);   // Signal that a full slot is available
            
            pause(1); // Producer runs fast to intentionally fill the buffer
        }
        
        wait(0); // Parent waits for consumer to finish reading all 20
        printf("Producer-Consumer completed successfully.\n");
    }
    exit(0);
}