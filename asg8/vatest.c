#include "types.h"
#include "stat.h"
#include "user.h"

int gvar=42;

int main(){
    int lvar=100;
    char *hvar=malloc(64);
    printf(1, "Global : VA = 0x%x -> PA = 0x%x\n", &gvar, va2pa((int)&gvar));
    printf(1, "Local  : VA = 0x%x -> PA = 0x%x\n", &lvar, va2pa((int)&lvar));
    printf(1, "Heap   : VA = 0x%x -> PA = 0x%x\n", hvar, va2pa((int)hvar));
    printf(1, "Invalid: VA = 0x7fffffff -> PA = %d\n", va2pa(0x7fffffff));
    if(fork() == 0){
        printf(1, "Child  : VA = 0x%x -> PA = 0x%x\n", &gvar, va2pa((int)&gvar));
    } else {
        wait();
        printf(1, "Parent : VA = 0x%x -> PA = 0x%x\n", &gvar, va2pa((int)&gvar));
    }
    exit();
}