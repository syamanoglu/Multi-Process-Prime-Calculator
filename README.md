# Multi-Process-Prime-Calculator
Multi-process program that finds and prints prime numbers up to given number with desired number of processes where communication is provided with unidirectional pipes.

The program takes two parameters N and M:
  N: largest integer in the integer sequence
  M: number of child processes
  
Program finds the prime numbers up to N(inclusive) with M number of processes and prints them in a different process.

In total: M child, 1 print, 1 main process, M + 2 process are created.

Communication between processes are provided with unidirectional pipes.

  
* The primes are computed as follows:

The Main process (MP) will generatea sequence of integers, starting with 2 and ending with N, like 2, 3, 4, 5, …. N. It will
send those integers one by one through the pipe between Main and C1. C1 will
receive these integers, and will identify the first integer in the sequence, 2, as a
prime number and will send the identified prime to PR through the respective pipe.
It will send the sequence that it is receiving to C2 while removing the identified
prime and all its multiples (2, 4, 6, 8,…). C1 does not have to wait until it receives the
whole sequence to start sending it to C2. C2 will receive the updated sequence and
will identify the first integer received as a prime (which is 3 in this case) and will
send the identified prime to PR through the respective pipe. C2 will send the
sequence it is receiving to C3 while removing all multiples of 3 (3, 9, 15, …). This will go on. 
CM will receive a sequence, will identify the first integer as a prime, will send
the prime to PR, and will send the sequence to Main while removing the multiples
of the identified prime. Main process will receive the sequence and will buffer the
sequence in a linked list (will not remove any integer). 

Main process will not start
sending the buffered sequence to C1 before it has finished sending the previous
sequence to C1. At the end of a sequence, the Main process may send a marker
integer, marking the end of the sequence. In this way, processes can understand the
end of a sequence and the start of a new sequence. This will go on until all primes in
range [2…N] are identified and printed out; that means until we arrive to a sequence
of length 0. At the end, all processes will terminate and the pipes will be removed.
