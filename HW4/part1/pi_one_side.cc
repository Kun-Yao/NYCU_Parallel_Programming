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

    MPI_Win win;

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
    long long int message = 0;
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

    MPI_Win_create(&local_count, sizeof(long long int), sizeof(long long int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

    if (world_rank == 0)
    {
        // Master
        global_count = local_count;
        for (int i = 1; i < world_size; i++) {
            long long int temp_count = 0;
            MPI_Win_lock(MPI_LOCK_SHARED, i, 0, win);
            MPI_Get(&temp_count, 1, MPI_LONG_LONG_INT, i, 0, 1, MPI_LONG_LONG_INT, win);
            MPI_Win_unlock(i, win);
            global_count += temp_count;
        }
    }
    else
    {
        // Workers
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, world_rank, 0, win);
        MPI_Put(&local_count, 1, MPI_LONG_LONG_INT, world_rank, 0, 1, MPI_LONG_LONG_INT, win);
        MPI_Win_unlock(world_rank, win);
    }

    MPI_Win_free(&win);

    if (world_rank == 0)
    {
        // TODO: handle PI result
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