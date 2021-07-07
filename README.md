# SO
## Operating Systems Class  - Project

Project carried out for the Operating Systems course that aims to use the UNIX programming language.
There were several tools such as:
processes, threads, pipes, sockets, etc.

###### This project uses UNIX C programming language

###### First Phase : This project uses processes and splits an array so every child process can sort the array.

main1.c - Splits the array, but the last child will sort everything.

main2.c - Comunication with pipes between child and father process

server1.c - Does the same thing but the communication between child and father is done by sockets

client1.c - Communication between child and father is done by sockets

###### Second Phase: Implementing the command find 

main1.c - Creates THREADS threads when a new directory is found it creates a new thread so it can find matches or new directories

main2.c - Creates N consumer threads and 1 producer thread - the consumer threads consumes the directory ant puts it in the buffer and the producer thread takes care of the
matches - This aims at solving the consumer-producer problem

main3.c - Creates N consumer threads and N producer threads - This has a problem that the matches are put in an array and the search takes way to long - fixable by using another data structure to save the matches - like a Separate Chaining Hash.

