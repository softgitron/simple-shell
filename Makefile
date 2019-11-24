compile-flags = -Wall -Werror -std=c11
debug-flags = -DDEBUG -g -O0
# Taken help for debug variables
# https://stackoverflow.com/questions/1079832/how-can-i-configure-my-makefile-for-debug-and-release-builds

wish: wish.o error.o main_loop.o string_manipulations.o run_programs.o pipes.o
	gcc wish.o error.o main_loop.o string_manipulations.o run_programs.o pipes.o -o wish $(compile-flags) -lreadline

wish-debug: compile-flags += $(debug-flags)
wish-debug: wish

wish.o: wish.c
	gcc wish.c -c -o wish.o $(compile-flags)

error.o: error.c
	gcc error.c -c -o error.o $(compile-flags)

main_loop.o: main_loop.c
	gcc main_loop.c -c -o main_loop.o $(compile-flags) -lreadline

string_manipulations.o: string_manipulations.c
	gcc string_manipulations.c -c -o string_manipulations.o $(compile-flags)

run_programs.o: run_programs.c
	gcc run_programs.c -c -o run_programs.o $(compile-flags)

pipes.o: pipes.c
	gcc pipes.c -c -o pipes.o $(compile-flags)

clean:
	rm -rf *.o
	rm ./wish