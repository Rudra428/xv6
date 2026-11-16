#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define n 4
#define m 4
#define fr(i,a,b) for(int i=a;i<b;i++)

int wfg[n][n], vis[n], path[n], plen;

int dfs(int u) {
    vis[u]=1;
    path[plen++]=u;
    fr(v,0,n){
        if(wfg[u][v]){
            if(vis[v]==1){
                int i=0;
                while(path[i]!=v){
                    i++;
                }
                fr(k,i,plen)printf("P%d -> ", path[k]);
                printf("P%d\n", v);
                return 1;
            }
            else if(vis[v]==0 && dfs(v))return 1;
        }
    }
    plen--;
    vis[u]=2;
    return 0;
}

void detect(int alloc[n][m], int req[n][m]) {
    // 1. Build Wait-For Graph: edge i -> j if Pi requests k and Pj holds k
    fr(i, 0, n) fr(j, 0, n) {
        wfg[i][j] = 0;
        fr(k, 0, m) {
            if (i != j && req[i][k] && alloc[j][k])
                wfg[i][j] = 1;
        }
    }

    // Print Wait-For Graph edges
    printf("Wait-For Graph edges: ");
    fr(i, 0, n) fr(j, 0, n)
        if (wfg[i][j]) printf("(P%d->P%d) ", i, j);
    printf("\n");

    // 2. Run DFS cycle detection
    fr(i, 0, n) vis[i] = 0;
    plen = 0;

    fr(i, 0, n) {
        if (vis[i] == 0 && dfs(i)) return;
    }
    printf("No deadlock detected (graph is acyclic).\n");
}

int main() {
    // Allocation matrix: P0 holds R0, P1 holds R1, P2 holds R2, P3 holds R3
    int alloc[n][m] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    // Scenario 1: Acyclic (P0->P1->P2->P3, no cycle)
    int req1[n][m] = {
        {0, 1, 0, 0}, // P0 wants R1 (held by P1)
        {0, 0, 1, 0}, // P1 wants R2 (held by P2)
        {0, 0, 0, 1}, // P2 wants R3 (held by P3)
        {0, 0, 0, 0}  // P3 wants nothing
    };
    printf("--- Scenario 1: No Deadlock ---\n");
    detect(alloc, req1);

    // Scenario 2: 3-process circular wait (P1 -> P2 -> P3 -> P1)
    int req2[n][m] = {
        {0, 1, 0, 0}, // P0 wants R1 (held by P1)
        {0, 0, 1, 0}, // P1 wants R2 (held by P2)
        {0, 0, 0, 1}, // P2 wants R3 (held by P3)
        {0, 1, 0, 0}  // P3 wants R1 (held by P1) -> closes cycle P1->P2->P3->P1!
    };
    printf("\n--- Scenario 2: Deadlock (3-Process Cycle) ---\n");
    detect(alloc, req2);

    exit(0);
}