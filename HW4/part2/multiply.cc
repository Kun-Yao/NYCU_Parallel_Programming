#include <mpi.h>
#include <fstream>
#include <iostream>

using namespace std;
// Read size of matrix_a and matrix_b (n, m, l) and whole data of matrixes from in
//
// in:        input stream of the matrix file
// n_ptr:     pointer to n
// m_ptr:     pointer to m
// l_ptr:     pointer to l
// a_mat_ptr: pointer to matrix a (a should be a continuous memory space for placing n * m elements of int)
// b_mat_ptr: pointer to matrix b (b should be a continuous memory space for placing m * l elements of int)
void construct_matrices(std::ifstream &in, int *n_ptr, int *m_ptr, int *l_ptr,
                        int **a_mat_ptr, int **b_mat_ptr)
{
    if (!in.is_open()) {
        throw std::runtime_error("File not opened");
    }

    int world_rank, world_size;
    int rows_message, begin_message;
    int n, m, l;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0){
        in >> *n_ptr >> *m_ptr >> *l_ptr;
        
        n = *n_ptr;
        m = *m_ptr;
        l = *l_ptr;
        // printf("%d, %d, %d\n", n, m, l);

        *a_mat_ptr = (int *)calloc(n * m, sizeof(int));

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                in >> (*a_mat_ptr)[i * m + j];
            }
        }
        
        *b_mat_ptr = (int *)calloc(m * l, sizeof(int));

        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < l; ++j) {
                in >> (*b_mat_ptr)[i * l + j];
            }
        }
    }
}

// Just matrix multiplication (your should output the result in this function)
// 
// n:     row number of matrix a
// m:     col number of matrix a / row number of matrix b
// l:     col number of matrix b
// a_mat: a continuous memory placing n * m elements of int
// b_mat: a continuous memory placing m * l elements of int
void matrix_multiply(const int n, const int m, const int l,
                     const int *a_mat, const int *b_mat)
{
    int world_rank, world_size;
    int rows_r, rows_q, rows, start=0;
    int *local_a, *local_b, *local_c, *c_mat;
    int N, M, L;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    MPI_Status status;
    MPI_Request requests;
    if (world_rank == 0){
        rows_q = n / world_size;
        rows_r = n % world_size;
        rows = (0 < rows_r) ? rows_q + 1 : rows_q;
        // printf("rows of rank 0 = %d", rows);

        c_mat = (int *)calloc(n * l, sizeof(int));
        for (int k = 0; k < l; k++){
            for (int i = 0; i < rows; i++){
                for (int j = 0; j < m; j++){
                    c_mat[i * l + k] += a_mat[i * m + j] * b_mat[j * l + k];
                }
            }
        }

        for (int i = 1; i < world_size; i++){
            MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&m, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&l, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            
            start += rows;
            rows = (i < rows_r) ? rows_q + 1 : rows_q;
            // printf("start = %d, rows = %d\n", start, rows);
            MPI_Send(&rows, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&a_mat[start * m], rows * m, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&b_mat[0], m * l, MPI_INT, i, 0, MPI_COMM_WORLD);
            // MPI_Wait(requests, status);
        }

        //receive compute results from other workers
        for (int i = 1; i < world_size; i++){
            MPI_Recv(&rows, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            // printf("rank = %d, start = %d\n", i, start);
            MPI_Recv(&c_mat[start * l], n * l, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        }
        
        //print c_mat
        for (int i = 0; i < n; i++){
            for (int j = 0; j < l; j++){
                printf("%d ", c_mat[i * l + j]);
                // if (j != l - 1) printf(" ");
            }
            printf("\n");
        }
        free(c_mat);
    }
    else{
        MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&M, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&L, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        local_a = (int *)calloc(N * M, sizeof(int));
        local_b = (int *)calloc(M * L, sizeof(int));
        local_c = (int *)calloc(N * L, sizeof(int));

        MPI_Recv(&rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&local_a[0], N * M, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        // printf("rank = %d, local_a[0] = %d\n", world_rank, local_a[0]);
        MPI_Recv(&local_b[0], M * L, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        // MPI_Wait(&requests, &status);

        for (int k = 0; k < L; k++){
            for (int i = 0; i < rows; i++){
                for (int j = 0; j < M; j++){
                    local_c[i * L + k] += local_a[i * M + j] * local_b[j * L + k];
                }
                // printf("%d ", local_c[i * L + k]);
            }
        }

        MPI_Send(&rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&local_c[0], rows * L, MPI_INT, 0, 0, MPI_COMM_WORLD);
    //     MPI_Wait(requests, status);
    free(local_a);
    free(local_b);
    free(local_c);
    }
    
    
}

void destruct_matrices(int *a_mat, int *b_mat)
{
    int world_rank, world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if (world_rank == 0){
        free(a_mat);
        free(b_mat);
    }
}