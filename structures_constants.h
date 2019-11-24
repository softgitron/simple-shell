#define _GNU_SOURCE
#include <limits.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>

#ifndef STRUCTURES_CONSTANTS_H
#define STRUCTURES_CONSTANTS_H

#define ERROR_MESSAGE "An error has occurred\n"
#define MAX_PIDS 20
#define MAX_ARGUMENTS 101
#define BUFFER_SIZE 1024
#define DEFAULT_PATHS "/bin"

#define MODE_NORMAL 1
#define MODE_REDIRECT 2
#define MODE_PIPE 3

typedef struct Environment {
    /* Paths to be searched like /bin /usr/bin. */
    char *paths;
    /* Current working directory. */
    char cwd[PATH_MAX];
    /* Should path be freed? */
    bool path_set_by_user;
} Environment;

typedef struct Process
{
    /* Process id. */
    pid_t pid;
    int stdin[2];
    int stdout[2];
    int stderr[2];
    int mode;
    int redirection;
} Process;

typedef struct Launch
{
    struct Environment *environment;
    char *command;
    char *args;
    char *redirection;
} Launch;

typedef struct PreLaunch
{
    struct Process *process;
    char **argv;
    char *path_to_run;
} PreLaunch;


#endif