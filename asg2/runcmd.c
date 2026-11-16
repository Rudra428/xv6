*#include<errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

    int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Incorrect number of arguments\n");
        return EXIT_FAILURE;
    }

    pid_t child_pid = fork();
    if (child_pid < 0)
    {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (child_pid == 0)
    {
        execvp(argv[1], &argv[1]);
        perror("execvp");
        _exit(127);
    }

    int status;
    pid_t waited_pid;
    do
    {
        waited_pid = waitpid(child_pid, &status, 0);
    } while (waited_pid < 0 && errno == EINTR);

    if (waited_pid < 0)
    {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
        printf("Command successfully completed\n");
        return EXIT_SUCCESS;
    }

    if (WIFEXITED(status))
    {
        fprintf(stderr, "Command exited with status %d\n", WEXITSTATUS(status));
        return WEXITSTATUS(status);
    }

    if (WIFSIGNALED(status))
    {
        fprintf(stderr, "Command terminated by signal %d\n", WTERMSIG(status));
        return EXIT_FAILURE;
    }

    fprintf(stderr, "Command did not complete successfully\n");
    return EXIT_FAILURE;
}
