#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void work(int x){
    volatile int c;
    x*=10000000;
    for(int i=0;i<x;i++){
        c++;
    }
}
int main(){
    int at[]={0,10,0};
    int bt[]={20,15,6};
    for(int i=0;i<3;i++){
        int p=fork();
        if(p==0){
            int pid=getpid();
            setbt(pid,bt[i]);
            pause(2);;
            printf("P %d arrived %d\n",pid,bt[i]);
            work(bt[i]);
            printf("P %d departed %d\n",pid,bt[i]);
            exit(0);
        }
        if(at[i]>0)pause(at[i]);
    }

    for(int i=0;i<3;i++){
        wait(0);
    }
    printf("happy happy happy\n");
}