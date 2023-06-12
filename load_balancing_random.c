//Randomized load balance
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define TASK_TAG 1
#define RESULT_TAG 2
#define TERMINATE_TAG 3

//Pure round robin 

int performTask(int n) {
    // Perform the task
    return n + n;
}

int main(int argc, char** argv) {
    int rank, size;
    double start_time, end_time;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        printf("This program requires at least 2 processes.\n");
        MPI_Finalize();
        return 1;
    }

    //set the seed with unix timestamp to gurantee different results
    srand(time(0)); 

    start_time = MPI_Wtime();  // Record the start time

    if (rank == 0) {
        // Master process
        int totalTasks = 1000;
        int numWorkers = size - 1;
        int randomWorker = (rand() % numWorkers) + 1;

        // Distribute tasks to worker processes
        for (int i = 0; i < totalTasks; i++) {
            int task = i +1;
            MPI_Send(&task, 1, MPI_INT, randomWorker, TASK_TAG, MPI_COMM_WORLD);
            randomWorker = (rand() % numWorkers) + 1;
        }

        // Receive results from worker processes
        for (int i = 0; i < totalTasks; i++) {
            int resultMessage[2];
            MPI_Recv(resultMessage, 2, MPI_INT, MPI_ANY_SOURCE, RESULT_TAG, MPI_COMM_WORLD, &status);
            int worker = status.MPI_SOURCE;  // Get the worker process rank
            int task = resultMessage[0];
            int result = resultMessage[1];
            // Process the received result
            printf("Task %d (from worker %d): Result = %d\n", task, worker, result);
        }

        // Send termination signal to worker processes
        for (int worker = 1; worker < size; worker++) {
            MPI_Send(NULL, 0, MPI_INT, worker, TERMINATE_TAG, MPI_COMM_WORLD);
        }

        end_time = MPI_Wtime();  // Record the end time
        //Print the execution time
        double execution_time = end_time - start_time;
        printf("Total execution time: %.2f seconds\n", execution_time);

    } else {
        // Worker processes
        while (1) {

            int task;
            MPI_Recv(&task, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == TERMINATE_TAG) {
                break;  // Terminate the worker process
            }

            // Perform the task
            int result = performTask(task);

            int resultMessage[2] = {task, result};
            // Send the result back to the master process
            MPI_Send(resultMessage, 2, MPI_INT, 0, RESULT_TAG, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}