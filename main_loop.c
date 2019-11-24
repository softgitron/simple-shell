#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>

#include "main_loop.h"
#include "run_programs.h"
#include "structures_constants.h"
#include "string_manipulations.h"
#include "error.h"

void run_shell(char *batch_name)
{
    FILE *file = NULL;
    char *line;
    Environment environment;
    environment.paths = DEFAULT_PATHS;
    environment.path_set_by_user = false;
    environment.cwd[0] = '\0';
    /* https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program */
    if (getcwd(environment.cwd, PATH_MAX) == NULL)
    {
        error("Can't get current working directory.\n");
        exit(0);
    }

    /* Open file if patch file operation. */
    if (batch_name != NULL)
    {
        if ((file = fopen(batch_name, "r")) == NULL) {
            error(ERROR_MESSAGE);
            exit(1);
        }
    }
    while (true)
    {
        line = get_next_input(batch_name, file);
        if (line == NULL) {
            break;
        }
        else if (line[0] == '\0') {
            /* If there is nothing to be processed, just continue. */
        }
        else if (line[0] == '&') {
            /* Check for nonsense command character at the beginning. */
        }
        /* Check buildin commands. */
        else if (strncmp(line, "exit", 4) == 0)
        {
            /* Check there is no parameters for exit build in. */
            if (line[4] != '\0') {
                error(ERROR_MESSAGE);
            } else {
                free(line);
                break;
            }
        }
        else if (strncmp(line, "path", 4) == 0)
        {
            if (environment.path_set_by_user == true)
            {
                /* Cleanup previous path memory. */
                free(environment.paths - 5);
            }
            environment.paths = line + 5;
            environment.path_set_by_user = true;
            /* Continue so new path variable memory will be reserved. */
            continue;
        }
        else if (strncmp(line, "cd ", 3) == 0)
        {
            /* https://stackoverflow.com/questions/12510874/how-can-i-check-if-a-directory-exists */
            /* Check does new directory exist. */
            char new_cwd[PATH_MAX];
            get_absolute_path(line + 3, environment.cwd, new_cwd);
            DIR *dir = opendir(new_cwd);
            if (dir)
            {
                strncpy(environment.cwd, new_cwd, PATH_MAX);
                chdir(new_cwd);
                closedir(dir);
            }
            else
            {
                error(ERROR_MESSAGE);
            }
        } else
        {
            handle_commands(line, &environment);
        }
        /* Free dynamically allocated line variable. */
        free(line);
    }
    /* Close batch file if there is one. */
    if (file != NULL)
    {
        fclose(file);
    }
    /* Cleanup user path. */
    if (environment.path_set_by_user == true) {
        free(environment.paths - 5);
    }
}

char *get_next_input(char *batch_name, FILE *file)
{
    char *line = NULL;
    size_t length = 0;
    if (batch_name == NULL)
    {
        /* Get input directly from user if there is no argument to shell. */
        line = readline("wish> ");
    }
    else
    {
        /* return next line of the file. */
        if (getline(&line, &length, file) == -1)
        {
            /* Enf of file reached. */
            free(line);
            return NULL;
        }
    }
    clean_line(line);
    return line;
}