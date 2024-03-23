# Mirror files

+ In Task 1, only one option can be chosen to be implemented:
either fopen(), fseek(), fread(), fwrite(), fclose()
or open(), lseek(), read(), write(), close()
The selected option should be processed in two ways:

  + Reading 1 character at a time.
Reading blocks of 1024 characters (the resulting file should be identical to that of variant 1).
A measurement of the execution time should be carried out for both implementation methods. The results should be presented in the form of file measurement_task_2.txt 

  + Task 1 Write a program that copies the contents of one file into another, reversed byte by byte.

  + Directions: Calls like fseek(infile, +1024, SEEK_END) or lseek(in, +1024, SEEK_END) are perfectly legal and have no side effects. In order to backtrack to the beginning of the previous block after reading a block of characters, the doubled block length with a minus sign must be specified as the second argument of the function fseek(..., ..., SEEK_CUR) or lseek(...., ..., SEEK_CUR). Verify the operation of the program as follows: 1) reverse the short text file, preview the result, check especially the starting and ending characters. 2) ./reverse file_binary tmp1 ; ./reverse tmp1 tmp2 ; diff -s tmp2 file_binary 3) you can also compare (diff -s) the result of the program and the result of the command tac < file_input | rev > file_output


+ Task 2 Write a program to browse the current directory using the opendir(), readdir() and stat() functions. For each file found that is not a directory, i.e. !S_ISDIR(buffer_stat.st_mode), print the size and name of the file. In addition, the total size of all files should be printed at the end. Subdirectories should not be viewed! The total size of the files should be stored in a variable of type long and output by the format %lld.
