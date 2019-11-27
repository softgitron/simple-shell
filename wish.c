#include <stdlib.h>

#include "structures_constants.h"
#include "main_loop.h"
#include "error.h"

int main(int argc, char **argv)
{
    if (argc > 2)
    {
        error("Too many arguments.\n");
        exit(1);
    }
    if (argc == 1)
    {
        run_shell(NULL);
    }
    else
    {
        run_shell(argv[1]);
    }
}

/* void nullify(char* line) {
    int character = 0;
    while (line[character] != '\0') {
        if (line[character] == ' ']) {
            line[character] = '\0'
        }
    }
} */

/* bool check_for_char(char* line, const char mark) {
    int character = 0;
    while (line[character] != '\0') {
        if (line[character] == mark]) {
            return true;
        }
    }
    return false;
} */