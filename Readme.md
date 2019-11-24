# Simple shell called wish
One of the system programming course practice works.

## What is it
Wish is a simple unix shell that has some basic functionality. Wish is made according to Ostep project rules. See their [git](https://github.com/remzi-arpacidusseau/ostep-projects) for more information (folder *processes-shell*).

## Compilation
Project can be compilated by simply cloning this repo to local directory and then running command `make wish`. Compilation with debug flags can be initiated by command `make wish-debug`.

## Running and features

After compilation program can be run with command `./wish`.

Program has some basic functionality. It has three build in commands:
- path
- cd
- exit

With path command user can change your current search paths where programs should be searched for. Cd command may be used to change current working directory and exit command exits from the program immediately.

Program can run multiple programs at once and it support output redirection to file in a bash fashion.

Program supports also batch mode. User may start program with one parameter which is a file where all commands should be read.

#### Parallel example:
`wish> ls & uname -a`

#### Redirection example:
`wish> ls > ./list.txt & uname -a > ./my-name.txt`

#### Batch mode example:
`./wish my batch_job.sh`

## Notes
Since I have been using dynamic memory allocation with malloc lately I wanted to test is it possible to implement simple shell without any manual memory allocations with malloc. I found out it is, but that was one of the reasons some code became bit messy. Especially *run_programs.c* main function is way too complicated and may be implemented in a much better fashion if it would be implemented in two parts like parsing and then executing a new process in a completely different function. Still I think some of the functions worked really well like string manipulation functions that did couple of tricks to avoid any new allocations.

## Ending words
This program has a long way before it is going to be full featured shell. Still all the core components are already there so that is at least a good start.
