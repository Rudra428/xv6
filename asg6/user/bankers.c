#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define n 5
#define m 3
#define fr(i,a,b) for(int i=a;i<b;i++)

int alloc[n][m]={{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}};
int max[n][m]={{7,5,3}, {3,2,2}, {9,0,2}, {2,2,2}, {4,3,3}};
int avl[m]={3, 3, 2};
int need[n][m];
int is_safe(){
    int a[m],vis[n]={0},seq[n];
    fr(i,0,m)a[i]=avl[i];
    int c=0;
    while(c<n){

        int f=0;
        fr(i,0,n){
            if(!vis[i]){
                int ok=1;
                fr(j,0,m){
                    if(need[i][j]>a[j]){
                        ok=0;
                        break;
                    }
                }
                if(ok){
                    fr(j,0,m)a[j]+=alloc[i][j];
                    seq[c++]=i;
                    vis[i]=1;
                    f=1;
                }
            }
        }

        if(!f){
            printf("no safe seq exist!!!\n");
            return 0;
        }
    }
    printf("Safe seq:\n");
    fr(i,0,n){
        printf("p%d->",seq[i]);
    }
    printf("\n");
    return 1;
}

void is_ok(int p,int req[m]){
    printf("\nP%d requests (%d, %d, %d)\n", p, req[0], req[1], req[2]);
    int flag=0;
    fr(j,0,m){
        avl[j]-=req[j];
        alloc[p][j]+=req[j];
        need[p][j]-=req[j];
        if(avl[j]<0 || need[p][j]<0)flag=1;
    }
    if((!flag) &&is_safe()){
        printf("Request granted!\n");
    }
    else{
        printf("Request denied — would lead to unsafe state\n");
        fr(j,0,m){
            avl[j]+=req[j];
            alloc[p][j]-=req[j];
            need[p][j]+=req[j];
        }
    }
}
int main(){
    fr(i,0,n){
        fr(j,0,m){
            need[i][j]=max[i][j]-alloc[i][j];
        }
    }
    printf("--- Initial State ---\n");
    is_safe();
    // Scenario 1: P1 requests (1, 0, 2) -> Granted (Safe)
  int r1[m] = {1, 0, 2};
  is_ok(1, r1);

  // Scenario 2: P0 requests (0, 2, 0) -> Denied (Unsafe)
  int r2[m] = {0, 2, 0};
  is_ok(0, r2);

  exit(0);
}