#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct shared{
    int data;
    int rc;
};

#define mutex 0
#define write 1

void reader(int id,struct shared *shm){
    for(int i=0;i<3;i++){
        sem_wait(mutex);
        shm->rc++;
        if(shm->rc==1){
            sem_wait(write);
        }
        sem_signal(mutex);
        printf("Reader %d entered CS. shared_data = %d (Active Readers: %d)\n", id, shm->data, shm->rc);
        pause(4);
        printf("Reader %d leaving\n",id);
        sem_wait(mutex);
        shm->rc--;
        if(shm->rc==0){
            sem_signal(write);
        }
        sem_signal(mutex);
        pause(2);
    }
    exit(0);
}
void writer(int id,struct shared *shm){
    for(int i=0;i<3;i++){
        sem_wait(write);
        printf("Muhehehe!! writer %d is changing data\n", id);
        int temp=shm->data;
        shm->data=temp+1;
        pause(4);
        printf("Muhehehe!! writer %d is leaving\n", id);
        sem_signal(write);
        pause(2);
    }
    exit(0);
}

int main(){
    struct shared *shm=(struct shared *)shm_get();
    shm->data=shm->rc=0;
    sem_init(mutex,1);
    sem_init(write,1);
    printf("Starting Readers-Writers...\n\n");
    for(int i=0;i<3;i++){
        int pid=fork();
        if(pid==0){
            shm_get();
            reader(i,shm);
        }
    }
    for(int i=0;i<2;i++){
        int pid=fork();
        if(pid==0){
            shm_get();
            writer(i,shm);
        }
    }
    for(int i = 0; i < 5; i++) {
        wait(0);
    }
    printf("\nReaders-Writers completed successfully.\n");
}