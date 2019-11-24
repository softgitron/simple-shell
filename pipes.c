#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include "structures_constants.h"

int create_pipes(Process *process)
{
    /* Create three pipes for stdin, stdout and stderr. */
    int p1 = pipe(process->stdin);
    int p2 = pipe(process->stdout);
    int p3 = pipe(process->stderr);
    if (p1 != -1 && p2 != -1 && p3 != -1)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

void connect_pipes_child(Process *process)
{
    if (process->redirection == 0)
    {
        dup2(process->stdout[1], STDOUT_FILENO);
        dup2(process->stderr[1], STDERR_FILENO);
    }
    else
    {
        dup2(process->redirection, STDOUT_FILENO);
        dup2(process->redirection, STDERR_FILENO);
    }
    close(process->stdout[0]);
    close(process->stdout[1]);
    close(process->stderr[0]);
    close(process->stderr[1]);
    /* Stdin is just for future proofing. */
    dup2(process->stderr[0], STDIN_FILENO);
    close(process->stderr[0]);
    close(process->stderr[1]);
}

void pipe_to_std(int pipe, int destination)
{
    char buffer[BUFFER_SIZE];
    buffer[0] = '\0';
    int code = 1;
    while (true)
    {
        code = read(pipe, buffer, BUFFER_SIZE);
        if (code != -1 && code != 0) {
            write(destination, buffer, code);
        } else {
            break;
        }
    }
}