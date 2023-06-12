//This method skips the server in case it is busy until it finds an available one
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TASK_TAG 1
#define RESULT_TAG 2
#define TERMINATE_TAG 3

int performTask(int task) {
    // Placeholder for the actual task processing logic
    return task + task;
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

    start_time = MPI_Wtime();  // Record the start time

    if (rank == 0) {
        // Master process
        int totalTasks = 1000;
        int numWorkers = size - 1;
        int* busyWorkers = (int*)malloc(numWorkers * sizeof(int));
        int nextWorker = 0;
        int task = 1;
        int tasksAssigned = 0; // Counter for assigned tasks
        int tasksReceived = 0; // Counter for received results
        int tasksRemaining = totalTasks; // Counter for remaining tasks

        // Initialize busyWorkers array to 0
        for (int i = 0; i < numWorkers; ++i) {
            busyWorkers[i] = 0;
        }

        while (tasksReceived < totalTasks) {
            // Check if there are tasks available to be assigned
            if (tasksAssigned < totalTasks) {
                // Find the next available worker
                int workerFound = 0;
                int initialWorker = nextWorker; // Save the initial worker index

                while (!workerFound) {
                    if (!busyWorkers[nextWorker]) {
                        // Send the task to the next available worker process
                        MPI_Send(&task, 1, MPI_INT, nextWorker + 1, TASK_TAG, MPI_COMM_WORLD);
                        // Mark the worker as busy
                        busyWorkers[nextWorker] = 1;

                        // Move to the next task
                        task++;

                        // Increment the tasks assigned counter
                        tasksAssigned++;
                        tasksRemaining--;
                        workerFound = 1; // Worker found, exit the loop
                    }

                    // Move to the next worker in a cyclic manner
                    nextWorker = (nextWorker + 1) % numWorkers;

                    // If we have traversed all workers and haven't found an available one,
                    // break the loop to avoid an infinite loop when all workers are busy
                    if (nextWorker == initialWorker)
                        break;
                }
            }

            // Receive results from any worker
            int resultMessage[2];
            MPI_Recv(resultMessage, 2, MPI_INT, MPI_ANY_SOURCE, RESULT_TAG, MPI_COMM_WORLD, &status);
            int worker = status.MPI_SOURCE;  // Get the worker process rank
            int taskReceived = resultMessage[0];
            int result = resultMessage[1];
            // Process the received result
            printf("Task %d (from worker %d): Result = %d\n", taskReceived, worker, result);
            busyWorkers[worker - 1] = 0; // Mark the worker as available

            // Increment the tasks received counter
            tasksReceived++;
        }

        // Send termination signal to worker processes
        for (int worker = 1; worker < size; worker++) {
            MPI_Send(NULL, 0, MPI_INT, worker, TERMINATE_TAG, MPI_COMM_WORLD);
        }

        free(busyWorkers);

        end_time = MPI_Wtime();  // Record the end time
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
            // Send the result and task number back to the master process
            int resultMessage[2] = {task, result};
            MPI_Send(resultMessage, 2, MPI_INT, 0, RESULT_TAG, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
