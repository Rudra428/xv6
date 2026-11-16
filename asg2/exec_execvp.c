#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    char *args[] = {"ls", "-l", NULL};

    execvp("ls", args);
    perror("execvp");
    return EXIT_FAILURE;
}
