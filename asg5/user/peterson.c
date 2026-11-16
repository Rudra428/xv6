#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct shared{
    int flag[2];
    int turn;
    int counter;
};


int main(){
    struct shared *shm=(struct shared *)shm_get();
    shm->flag[0]=shm->flag[1]=shm->turn=shm->counter=0;
    int pid=fork();
    if(pid<0){
        printf("failed!!\n");
        exit(1);
    }
    if(pid==0){
        shm_get();
    }
    int process=(pid==0)?1:0;
    for(int i=0;i<10;i++){
        //entry section
        shm->flag[process]=1;
        __sync_synchronize();
        shm->turn=process;
        __sync_synchronize();
        int other=1-process;
        __sync_synchronize();
        while(shm->flag[other] && shm->turn==process){
            //eat 5star
        }
        __sync_synchronize();
        //cs
        int temp=shm->counter;
        temp++;
        shm->counter=temp;
        __sync_synchronize();
        printf("process %d in cs counter=%d\n",process,shm->counter);
        shm->flag[process]=0;
        __sync_synchronize();
        pause(2);
    }
    if(pid!=0)wait(0);
    exit(0);
}