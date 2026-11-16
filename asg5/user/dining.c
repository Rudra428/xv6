#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
    for(int i=0;i<5;i++){
        sem_init(i,1);
    }
    for(int i=0;i<5;i++){
        if(fork()==0){
            int f1=i;
            int f2=(i+1)%5;
            if(f1>f2){
                f1=f1^f2;
                f2=f1^f2;
                f1=f1^f2;
            }
            for(int j=0;j<5;j++){
                printf("Philosopher %d: THINKING -> HUNGRY\n", i);
                sem_wait(f1);
                sem_wait(f2);

                printf("Philosopher %d: EATING\n", i);
                pause(2);

                sem_signal(f2);
                sem_signal(f1);
                printf("Philosopher %d: THINKING\n", i);
                pause(2);
            }
            exit(0);
        }
    }
    for(int i=0;i<5;i++){
        wait(0);
    }
    exit(0);
}