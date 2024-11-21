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
    MPI_Status status;

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

    // TODO: binary tree redunction
    int nodes = world_size;
    while(nodes > 1){
        if (world_rank >= (nodes / 2)){
            dest = world_rank - (nodes / 2);
            MPI_Send(&local_count, 1, MPI_LONG_LONG_INT, dest, tag, MPI_COMM_WORLD);
            break;
        }
        else{
            long long int message;
            source = world_rank + (nodes / 2);
            MPI_Recv(&message, 1, MPI_LONG_LONG_INT, source, tag, MPI_COMM_WORLD, &status);
            local_count += message;
        }
        nodes /= 2;
    }

    if (world_rank == 0)
    {
        // TODO: PI result
        pi_result = 4 * local_count / (( double ) tosses);
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
