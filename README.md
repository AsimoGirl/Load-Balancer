# Load Balancer For Distributed Systems
Load balancer implementations with various algorithms

In order to compile the code you must have MPI installed

It is compiled with the command
```c
mpicc <file_name>.c -o <file_name>
```

And it is executed with the command
```c
mpirun -n <number_processes> <file_name>
```

The file load_balancing_least.c corresponds to the implementation of the least connections algorithm.

The file load_balancing_random.c corresponds to the implementation of the random algorithm.

The file load_balancing_rr_pure.c corresponds to the implementation of the round robin algorithm.

The files load_balancing_rr_list1.c and load_balancing_rr_wait1.c correspond to edited versions of the round robin algorithm, these versions were much slower than the original round robin so they were not considered in the final document.

