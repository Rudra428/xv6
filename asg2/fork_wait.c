#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (child_pid == 0) {
        printf("I am child, PID: %ld\n", (long)getpid());
        return EXIT_SUCCESS;
    }

    int status;
    pid_t waited_pid;
    do {
        waited_pid = waitpid(child_pid, &status, 0);
    } while (waited_pid < 0 && errno == EINTR);

    if (waited_pid < 0) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    printf("I am parent, PID: %ld\n", (long)getpid());
    return EXIT_SUCCESS;
}
