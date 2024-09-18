#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(){
    long long int toss, number_in_circle = 0;
    srand( time(NULL) );
    long long int number_of_tosses = rand() + 1;
    double min = -1.0, max = 1.0;
    for (toss = 0; toss < number_of_tosses; toss ++) { 
        double x = (max - min) * rand() / (RAND_MAX + 1.0) + min;
        double y = (max - min) * rand() / (RAND_MAX + 1.0) + min;
        double distance_squared = x * x + y * y;
        if ( distance_squared <= 1)
            number_in_circle++;
    }
    double pi_estimate = 4 * number_in_circle / (( double ) number_of_tosses);
    printf("PI = %.2f", pi_estimate);
}