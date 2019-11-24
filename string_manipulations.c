#define _GNU_SOURCE
#include <string.h>
#include <limits.h>

#include "structures_constants.h"
#include "string_manipulations.h"
#include "error.h"

void clean_line(char *line)
{
    /* If null return imidiately. */
    if (line == NULL)
    {
        return;
    }
    if (line[0] == '\0') {
        return;
    }
    /* Cleanup white spaces and and new line characters. */
    /* Cleanup whitespaces at the beginning. */
    int amount = 0;
    int character = 0;
    int last_non_space = -1;
    while (line[amount] == ' ')
    {
        amount++;
    }
    /* Move every character by specified amount. */
    while (line[character + amount] != '\0' && line[character + amount] != '\n')
    {
        line[character] = line[character + amount];
        if (line[character + amount] != ' ')
        {
            /* For trailing slash removal. */
            last_non_space = character;
        }
        character++;
    }
    /* Set terminator next to last needed character. */
    line[last_non_space + 1] = '\0';
}

void convert_to_arguments(char *string, char *args[], char *program)
{
    /* Convert normal string to list of arguments. */
    /* Set first argument always to program name. */
    args[0] = program;
    if (string == NULL)
    {
        args[1] = NULL;
        return;
    }
    if (string[0] == '\0') {
        args[1] = NULL;
        return;
    }
    char *next_parameter = string;
    int arguments = 1;
    int character = 0;
    while (string[character] != '\0')
    {
        if (string[character] == ' ')
        {
            if (arguments == MAX_ARGUMENTS - 2)
            {
                args[MAX_ARGUMENTS - 1] = NULL;
                error("Can't set more than 100 arguments");
                return;
            }
            string[character] = '\0';
            args[arguments] = next_parameter;
            /* Skip that one space. */
            next_parameter = string + character + 1;
            arguments++;
        }
        character++;
    }
    args[arguments] = next_parameter;
    args[arguments + 1] = NULL;
}

int handle_relative_path(char *path, char *cwd, char *results)
{
    /* Relative path 0, normal path 1, null, 2. */
    results[0] = '\0';
    if (path[0] != '\0')
    {
        if (path[0] == '.' && path[1] == '/')
        {
            strcat(results, cwd);
            strcat(results, "/");
            strncat(results, path + 2, PATH_MAX - strnlen(cwd, PATH_MAX - 1) - 1);
            return 0;
        }
        else
        {
            strncat(results, path, PATH_MAX);
            return 1;
        }
    }
    return 2;
}

void get_absolute_path(char *path, char *cwd, char *results) {
    results[0] = '\0';
    if (handle_relative_path(path, cwd, results) != 0 && path[0] != '/') {
        int cwd_length = strnlen(cwd, PATH_MAX);
        strcpy(results, cwd);
        strcat(results, "/");
        strncat(results, path, PATH_MAX - cwd_length - 1);
        clean_trailing_slash(results);
    }
}

void clean_trailing_slash(char *path) {
    int last = strnlen(path, PATH_MAX);
    if (path[last] == '/') {
        path[last] = '\0';
    }
}

int check_for_char(char *string, char character) {
    int point = 0;
    int count = 0;
    while (string[point] != '\0') {
        if (string[point] == character) {
            count++;
        }
        point++;
    }
    return count;
}