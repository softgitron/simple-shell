#include <stdio.h>

void error(char *message)
{
    /*https://stackoverflow.com/questions/39002052/how-i-can-print-to-stderr-in-c/39002243*/
    fprintf(stderr, "%s", message);
}