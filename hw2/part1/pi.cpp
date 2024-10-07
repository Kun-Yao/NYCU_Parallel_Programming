#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    srand(time(NULL));
    long long int number_in_circle = 0;
    long long int number_of_tosses = atoll(argv[2]);
    double x, y, distance_squared, pi_estimate;
    int min = -1, max = 1;
    for (int toss = 0; toss < number_of_tosses; toss++)
    {
        x = (max - min) * rand() / (RAND_MAX + 1.0) + min;
        y = (max - min) * rand() / (RAND_MAX + 1.0) + min;
        distance_squared = x * x + y * y;
        if (distance_squared <= 1)
            number_in_circle++;
    }
    pi_estimate = 4 * number_in_circle / ((double)number_of_tosses);
    printf("%lf\n", pi_estimate);
    return 0;
}