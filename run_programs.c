#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

#include "run_programs.h"
#include "string_manipulations.h"
#include "error.h"
#include "pipes.h"

void handle_commands(char *line, Environment *environment)
{
    /* Go line by character by character until end of string or special operator. */
    int character = 0;
    char *program = line;
    char *args = NULL;
    char *redirection = NULL;
    bool final_round = false;
    bool args_set = false;
    bool redirections_set = false;
    int pid_number = 0;
    Process process[MAX_PIDS];
    while (true)
    {
        if (line[character] == '&' || line[character] == '\0')
        {
            /* If there is real orginal null terminator set flag for break. */
            if (line[character] == '\0') {
                final_round = true;
            }
            else if (line[character + 1] == '\0' && line[character] == '&')
            {
                /* If final is &. */
                final_round = true;
            }
            line[character] = '\0';
            /* Check do we have reatched max amount of pids. */
            if (pid_number != MAX_PIDS - 1)
            {
                /* Cleanup all parameters and convert arguments to real argument format. */
                clean_line(program);
                clean_line(args);
                clean_line(redirection);
                Launch launch;
                launch.environment = environment;
                launch.command = program;
                launch.args = args;
                launch.redirection = redirection;
                process[pid_number] = run_program(&launch);
                /* Mark mode as apropriate. */
                if (redirections_set == true)
                {
                    process[pid_number].mode = MODE_REDIRECT;
                }
                else
                {
                    process[pid_number].mode = MODE_NORMAL;
                }
                if (process[pid_number].pid != 0)
                {
                    /* Raise pid number only if there was no error. */
                    pid_number++;
                }
            }
            else
            {
                error("We have allready reatched max amount of pids.\n");
            }
            /* Reset all parameters for next sentence. */
            program = line + character + 1;
            args_set = false;
            redirections_set = false;
            redirection = NULL;
            args = NULL;
            /* Skip until there is no space. This is important when running multiple commands. */
            character++;
            while (true)
            {
                if (line[character] == ' ')
                {
                    character++;
                }
                else
                {
                    /* Degrement one since there is alway ingrement at the end. Litle hacky but quite clear. */
                    character--;
                    break;
                }
            }
            /* End if there was real null at the end. */
            if (final_round == true)
                break;
        }
        /* And character may be now checked safely since null terminator is allready checked. */
        else if (line[character] == ' ' && args_set == false && line[character + 1] != '&')
        {
            args = line + character + 1;
            line[character] = '\0';
            args_set = true;
        }
        else if (line[character] == '>')
        {
            /* Check for multiple redirection signs. */
            if (redirections_set == false)
            {
                redirection = line + character + 1;
                line[character] = '\0';
                redirections_set = true;
            }
            else
            {
                error("There can be only one redirection sign.\n");
                break;
            }
        }
        character++;
    }
    handle_processes(process, pid_number);
}

Process run_program(Launch *launch)
{
    Process process;
    process.pid = 0;
    process.redirection = 0;
    char path_to_run[PATH_MAX];
    char *argv[MAX_ARGUMENTS];
    PreLaunch prelaunch;
    prelaunch.path_to_run = path_to_run;
    prelaunch.argv = argv;
    prelaunch.process = &process;
    if (program_pre_launch_preparations(launch, &prelaunch) != 0) {
        return process;
    }
    process.pid = fork();
    /* If the pid is 0 its child process. */
    if (process.pid == 0)
    {
        connect_pipes_child(&process);
        execv(path_to_run, argv);
        /* In case exec fails. */
        error("Creating new process failed.\n");
        exit(4);
    }
    else if (process.pid == -1)
    {
        /* Fork failed for some reson. */
        error(ERROR_MESSAGE);
        return process;
    }
    else
    { /* pid!=0; parent process */
        close(process.stdout[1]);
        close(process.stderr[1]);
        close(process.stdin[0]);
        return process;
    }
}

int program_pre_launch_preparations(Launch *launch, PreLaunch *prelaunch) {
    /* Zero success, one fail. */
    convert_to_arguments(launch->args, prelaunch->argv, launch->command);
    if (handle_program_path(launch->command, prelaunch->path_to_run, launch->environment) != 0)
    {
        /* If we can't reach program then its error. */
        error("Couldn't find progam binary on launch.\n");
        return 1;
    }
    /* Prepare redirection file. */
    if (launch->redirection != NULL)
    {
        /* Check file is not empty. */
        if (launch->redirection[0] == '\0') {
            error("There has to be filename after redirection.\n");
            return 1;
        }
        /* Check that path does not contain spaces. */
        if (check_for_char(launch->redirection, ' ') != 0) {
            error("No spaces allowed in redirection.\n");
            return 1;
        }
        /* https://stackoverflow.com/questions/8516823/redirecting-output-to-a-file-in-c */
        char absolute_path[PATH_MAX];
        get_absolute_path(launch->redirection, launch->environment->cwd, absolute_path);
        if ((prelaunch->process->redirection = open(absolute_path, O_RDWR | O_CREAT | O_TRUNC, 0600)) == 0)
        {
            return 1;
        }
    }
    /* Crete pipes. */
    /* https://stackoverflow.com/questions/7292642/grabbing-output-from-exec */
    if (create_pipes(prelaunch->process) == -1)
    {
        return 1;
    }
    return 0;
}

int handle_program_path(char *program, char *path_to_run, Environment *environment)
{
    /* Zero success, one not found or not accessible. */
    struct stat info;
    int previous = 0;
    int count = 0;
    path_to_run[0] = '\0';
    if (handle_relative_path(program, environment->cwd, path_to_run) == 0)
    {
        if (stat(path_to_run, &info) != 0 && info.st_mode == S_IXUSR)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    while (true)
    {
        /* https://stackoverflow.com/questions/13098620/using-stat-to-check-if-a-file-is-executable-in-c */
        if (environment->paths[previous + count] == '\0' || environment->paths[previous + count] == ' ')
        {
            strncpy(path_to_run, environment->paths + previous, count + 1);
            path_to_run[count] = '\0';
            /* Fix if trailing slash is missing from the path. */
            if (environment->paths[previous + count - 1] != '/')
            {
                strcat(path_to_run, "/");
            }
            strncat(path_to_run, program, PATH_MAX - count - 2);
            if (stat(path_to_run, &info) == 0 && info.st_mode & S_IXUSR)
            {
                return 0;
            }
            else
            {
                /* Exit if it was final path option. */
                if (environment->paths[previous + count] == '\0')
                {
                    break;
                }
                else
                {
                    /* Prepare for next path. */
                    previous += count + 1;
                    count = 0;
                    continue;
                }
            }
        }
        count++;
    }
    return 1;
}

void handle_processes(Process *process, int pid_amount)
{
    /* Handle all processes and wait for them to finnish. */
    for (int number = 0; number < pid_amount; number++)
    {
        waitpid(process[number].pid, 0, 0);
        if (process[number].mode != MODE_REDIRECT)
        {
            /* Write output to a terminal. */
            pipe_to_std(process[number].stdout[0], STDOUT_FILENO);
            pipe_to_std(process[number].stderr[0], STDERR_FILENO);
        }
        else
        {
            close(process[number].redirection);
        }
        close(process[number].stdout[0]);
        close(process[number].stderr[0]);
        close(process[number].stdin[1]);
    }
}