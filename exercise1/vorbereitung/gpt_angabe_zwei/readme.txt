Server and Client Description

Server Generates a Random Number in a certain range.
Saves it and compares it.

Clients generate random numbers in a range and try to match the Servers one.

Clients write their numbers in a shared memory

Once number is hit all clients get terminated and server stops

Server writes the number of read solutions into a file if specified.
Otherwise into stdout.
Server generates a random number or takes one from the input.

If a new generator gets started with the argument -k the clients and server 
need to terminate.

In that case a Message should appear on every client.

The server should write a file, and says its been terminated to early and 
specify the number.

The clients are given 2 arguments -s for range start and -e for range end.

e.g. ./client [-k terminate all] -s number  -e number

./server [-f file name] [-n number]


To be done: 3 Semaphores (used, free, mutex)
used: how much space is used in the Buffer
free: how much space is available
mutex: to achieve mutual exclusivity while writing to buffer
1 Shared memory
Circular Buffer
