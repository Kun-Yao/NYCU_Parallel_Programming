#include <iostream>
#include <stdlib.h>
#include <random>
#include <time.h>
#include <pthread.h>
#include <smmintrin.h>
#include <immintrin.h>

using namespace std;

struct thread_arg{
    mt19937 gen;
    uniform_real_distribution<> dist;
    long long int tosses;
};
void* monte_carlo(void* args){
    thread_arg* t_arg = static_cast<thread_arg*>(args);
    long long int* local_num = new long long int(0);
    
    for (int toss = 0; toss < t_arg->tosses; toss += 4)
    {
        // pthread_t threadID = pthread_self();
        // cout << "tid=" << threadID << endl;
        // random vector x, vector y
        double vx[4], vy[4];
        for (int i = 0; i < 4; i++){
            vx[i] = t_arg->dist(t_arg->gen);
            vy[i] = t_arg->dist(t_arg->gen);
        }

        __m256d avx_ones = _mm256_set1_pd(1.0);

        // vector square_x = vector x * vector x
        __m256d avx_vx = _mm256_loadu_pd(vx);
        __m256d avx_mult_vx = _mm256_mul_pd(avx_vx, avx_vx);

        // vector square_y = vector y * vector y
        __m256d avx_vy = _mm256_loadu_pd(vy);
        __m256d avx_mult_vy = _mm256_mul_pd(avx_vy, avx_vy);

        // vector square_x + vector square_y
        __m256d avx_dist_square = _mm256_add_pd(avx_mult_vx, avx_mult_vy);
        
        // leq 1
        __m256d avx_leq1 = _mm256_cmp_pd(avx_dist_square, avx_ones, _CMP_LE_OQ);

        double result[4];
        _mm256_storeu_pd(result, avx_leq1);
        
        
        for (int i = 0; i < 4; i++){
            // cout << "result " << static_cast<long long int>(result[i]) << endl;
            *local_num += (static_cast<long long int>(result[i]) != 0);
        }
        // cout << "local_num " << *local_num << endl;
    }
    
    return local_num;
}

int main(int argc, char *argv[])
{
    // cout << "start" << endl;

    int NUM_THREADS = atoi(argv[1]);
    long long int number_in_circle = 0;
    long long int number_of_tosses = atoll(argv[2]);
    // cout << number_of_tosses << endl << endl;
    double pi_estimate;

    pthread_t threads[NUM_THREADS];
    thread_arg arg[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++){
        long long int each_tosses = number_of_tosses / NUM_THREADS;
        random_device rd;
        arg[i].gen = mt19937(rd());
        arg[i].dist = uniform_real_distribution<>(-1.0, 1.0);
        arg[i].tosses = each_tosses;

        if (pthread_create(&threads[i], nullptr, monte_carlo, &(arg[i]))){
            cerr << "Error creating thread" << i+1 << endl;
            return 1;
        }
    }       

    for (int i = 0; i < NUM_THREADS; i++){
        void* retval;
        if (pthread_join(threads[i], &retval)){
            cerr << "Error joining thread" << endl;
            return 2;
        }
        long long int* local_num = static_cast<long long int*>(retval);
        number_in_circle += *local_num;
        delete local_num;
    }

    pi_estimate = 4 * number_in_circle / ((double)number_of_tosses);
    cout << "pi=" << pi_estimate << endl;
    return 0;
}
