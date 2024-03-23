# Libraries:

+ Create a library in C exposing the following two functions to clients:
  + `int collatz_conjecture(int input)` - a function that implements a Collatz rule of the form:
Collatz Conjecture
This function takes one number of integer type. If it is even, divide it by 2 and return the result. If it is odd, multiply it by 3 and add 1 and return the result.
  + `int test_collatz_convergence(int input, int max_iter)` a function that checks after how many calls collatz_conjecture converges to 1.
It should call the Collatz rule first on the input number and then on the result obtained from the rule.
In order to protect the function from looping too long, the second parameter is an upper limit on the number of iterations.
In case the function performs the maximum number of iterations and does not find a result of 1, then return -1.

+ Create two entries in the makefile: for static compilation of the library and for shared compilation.
+ Write a client using the library code, the client should check a number of numbers using test_collatz_convergence, i.e. after how many iterations the result converges to 1 and print the number of iterations to the standard output. The client should use the library in 3 ways:
  + As a static library
  + As a shared library (dynamically linked)
  + As a dynamically loaded library
+ Create a corresponding entry in the Makefile for each variant. For the dynamic library implementation, use the definition of a constant (-D) and a preprocessor directive to modify the way the client works.
View the contents of the binary files of all client variants using objdump, find where the test_collatz_convergence function is called discuss the differences in the binary code. For greater code clarity compile without -O0 optimisation.
