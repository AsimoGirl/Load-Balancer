//This round robin keeps a list of the tasks and assigns them to 
//the servers without considering availability, pure round robin?

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TASK_TAG 1
#define RESULT_TAG 2
#define TERMINATE_TAG 3

#define MAX_TASKS 100

typedef struct {
    int task;
    int assigned;
} TaskInfo;

int performTask(int task) {
    // Placeholder for the actual task processing logic
    return task + task;
}

int main(int argc, char** argv) {
    int rank, size;
    double start_time, end_time;

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
        int numWorkers = size - 1;
        TaskInfo taskQueue[MAX_TASKS];
        int taskCount = 0;
        int nextWorker = 0;

        // Initialize taskQueue
        for (int i = 0; i < MAX_TASKS; i++) {
            taskQueue[i].task = i + 1;
            taskQueue[i].assigned = 0;
        }

        int terminateCount = 0;

        while (terminateCount < numWorkers && taskCount < MAX_TASKS) {
            // Check if there are available tasks
            if (taskCount < MAX_TASKS) {
                // Find the next available task
                int taskFound = 0;
                int initialTask = nextWorker; // Save the initial task index

                while (!taskFound) {
                    //Cycle through the tasks for the index
                    int taskIndex = nextWorker % MAX_TASKS;

                    if (!taskQueue[taskIndex].assigned) {
                        // Send the task to the next available worker process
                        MPI_Send(&taskQueue[taskIndex].task, 1, MPI_INT, nextWorker % numWorkers + 1, TASK_TAG, MPI_COMM_WORLD);
                        printf("Assigned task %d to worker %d\n", taskQueue[taskIndex].task, nextWorker % numWorkers + 1);

                        // Mark the task as assigned
                        taskQueue[taskIndex].assigned = 1;

                        // Move to the next task
                        taskCount++;
                        taskFound = 1; // Task found, exit the loop
                    }

                    // Move to the next task in a cyclic manner
                    nextWorker++;

                    // If we have traversed all tasks and haven't found an available one,
                    // break the loop to avoid an infinite loop when all tasks are assigned
                    if (nextWorker - initialTask >= MAX_TASKS)
                        break;
                }
            }

            // Receive results from any worker
            MPI_Status status;
            int result;
            int worker;
            MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, RESULT_TAG, MPI_COMM_WORLD, &status);
            worker = status.MPI_SOURCE;
            printf("Received result %d from worker %d\n", result, worker);

            // Find the corresponding task and mark it as unassigned
            for (int i = 0; i < MAX_TASKS; ++i) {
                if (taskQueue[i].task == result && taskQueue[i].assigned) {
                    taskQueue[i].assigned = 0;
                    break;
                }
            }

            // Check for termination signal
            if (status.MPI_TAG == TERMINATE_TAG) {
                terminateCount++;
                printf("Worker %d has terminated.\n", worker);
            }
        }

        // Send termination signal to worker processes
        for (int worker = 1; worker < size; ++worker) {
            int terminateTask = 0;
            MPI_Send(&terminateTask, 1, MPI_INT, worker, TERMINATE_TAG, MPI_COMM_WORLD);
        }

    } else {
        // Worker processes
        while (1) {
            int task;
            MPI_Recv(&task, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (task == 0) {
                // Terminate the worker process
                break;
            }

            // Perform the task
            int result = performTask(task);

            // Send the result back to the master process
            MPI_Send(&result, 1, MPI_INT, 0, RESULT_TAG, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    end_time = MPI_Wtime();  // Record the end time

    if (rank == 0) {
        double execution_time = end_time - start_time;
        printf("Total execution time: %.2f seconds\n", execution_time);
    }

    return 0;
}
