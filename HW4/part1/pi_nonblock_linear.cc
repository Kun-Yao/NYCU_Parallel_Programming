#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    // TODO: MPI init
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int source;
    int dest = 0;

    int tag=0;
    MPI_Status status[world_size - 1];
    
    long long int local_count = 0;
    long long int global_count = 0;
    long long int local_tosses = tosses / world_size;
    int SEED = 78;
    unsigned int seed = world_rank * SEED;
    srand(seed);
    for (long long int toss = 0; toss < local_tosses; toss++) { 
        double x = 2.0 * rand_r(&seed) / (RAND_MAX + 1.0) - 1.0;
        double y = 2.0 * rand_r(&seed) / (RAND_MAX + 1.0) - 1.0;
        double distance_squared = x * x + y * y;
        if (distance_squared <= 1)
            local_count++;
    }

    if (world_rank > 0)
    {
        // TODO: MPI workers
        MPI_Send(&local_count, 1, MPI_LONG_LONG_INT, dest, tag, MPI_COMM_WORLD);
    }
    else if (world_rank == 0)
    {
        // TODO: non-blocking MPI communication.
        // Use MPI_Irecv, MPI_Wait or MPI_Waitall.
        MPI_Request requests[world_size - 1];
        long long int* count = (long long int*)calloc(world_size - 1, sizeof(long long int));
        global_count = local_count;

        for (source = 1; source < world_size; source++){
            MPI_Irecv(&count[source - 1], 1, MPI_LONG_LONG_INT, source, tag, MPI_COMM_WORLD, &requests[source - 1]);
        }

        MPI_Waitall(world_size - 1, requests, status);

        for (int i = 0; i < world_size - 1; i++){
            global_count += count[i];
        }

        free(count);

    }

    if (world_rank == 0)
    {
        // TODO: PI result
        pi_result = 4 * global_count / (( double ) tosses);
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
