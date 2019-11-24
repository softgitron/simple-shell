#include "structures_constants.h"

void handle_commands(char *line, Environment *environment);
Process run_program(Launch *launch);
int progrma_pre_launch_preparations(Launch *launch, PreLaunch *prelaunch);
int handle_program_path(char *program, char *path_to_run, Environment *environment);
void handle_processes(Process *process, int pid_amount);