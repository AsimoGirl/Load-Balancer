# ProyectoSistemasDistribuidos
Implementaciones de balanceadores de carga con diversos algoritmos

Para poder compilar el código se debe tener instalado MPI

Se compila con el comando 
```c
mpicc <nombre_archivo>.c -o <nombre_archivo>
```

Y se ejecuta con el comando
```c
mpirun -n <numero_procesos> <nombre_archivo>
```

El archivo load_balancing_least.c corresponde a la implementación del algoritmo de conexiones mínimas.

El archivo load_balancing_random.c corresponde a la implementación del algoritmo aleatorio.

El archivo load_balancing_rr_pure.c corresponde a la implementación del algoritmo de round robin.

Los archivos load_balancing_rr_list1.c y load_balancing_rr_wait1.c corresponden a versiones editadas del algoritmo de round robin, estas versiones fueron mucho más lentas que el round robin orignal así que no se consideraron en el documento final.


