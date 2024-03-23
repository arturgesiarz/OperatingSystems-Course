# Processes

+ Write a program that takes one argument: argv[1]. The program is to create argv[1] child processes. Each child process is to print two identifiers on a single line to the standard output: the identifier of the parent process and its own. At the end of the standard output, the parent process is to print argv[1]

+ Write a program that takes one argument: argv[1] - the path of the directory. The program should print its name to the standard output, using the printf() function. Declare a global variable global and then a local variable local. Depending on the value returned by fork() perform error handling, execute the parent process / child process. In the child process:
display the "child process" message,
increment the global and local variables,
display the message "child pid = %d, parent pid = %d".
display message "child's local = %d, child's global = %d"
execute the /bin/ls program with argument argv[1], using the execl() function, returning its error code.
In the parent process:
display the message "parent process"
display the message 'parent pid = %d, child pid = %d'.
display the message 'child exit code: %d'.
display the message 'parent's local = %d, parent's global = %d'.
return the relevant error code.
