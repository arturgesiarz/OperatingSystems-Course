# Makefile:

+ Write a simple countdown.c program that counts down from 10 to 0 in a loop and prints the current number to the console (each number on a new line).

+ Create a Makefile with which to compile your program.
The Makefile should contain at least three targets: 
all, countdown, clean.
  + all - should build all Targets (only countdown for now, we will add new Targets in future tasks).
  + countdown - builds the countdown.c program
  + clean - removes all binary files, clears project state

+ Use the CC and CFLAGS variables to define the compiler (gcc) and compilation flags (-Wall, -std=c17, ...). 
In addition, there should be a special target .PHONY in the Makefile.
Compile and run the program.
