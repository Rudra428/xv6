#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    execl("/bin/ls", "ls", "-l", (char *)NULL);
    perror("execl");
    return EXIT_FAILURE;
}
