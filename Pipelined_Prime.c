#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <stdbool.h>


bool is_prime(int num) {  //Used for the sequential code, checks if a number is prime
    if (num < 2) return false; 
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return false;
        }
    }
    return true; 
}


void find_first_n_primes(int n) { // Used for sequential code, finds the first n primes sequentially
    int count = 0;
    int num = 2;

    while (count < n) {
        if (is_prime(num)) {
            printf(" ");
            count++;
        }
        num++;
    }
}

int main(int argc, char **argv) {
    int rank, size, n;
    double serial_start, parallel_start ; //serial_start and parallel_start are the starting time of the serial and parallel timer respectively
    double serial_end, parallel_end; //serial_end and parallel_end are the time at which  the serial and parallel timer  ends respectively
    double serial_time, parallel_time, speedup; // speedup is equal to the serial_time over parallel_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    n = size; // In our case n will be equal to the size
    if (rank == 0) {
        parallel_start = MPI_Wtime();
        for (int i = 3; i <= n * n; i++) { 
            if (i % 2 != 0) {
                MPI_Send(&i, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            }
        }

        int terminator = -1;
        MPI_Send(&terminator, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        parallel_end = MPI_Wtime();
        parallel_time = parallel_end - parallel_start;
        printf("\nUSING PARALLEL APPROACH:\n\n");
        printf("The total time is: %f\n\n\n", parallel_time);

        serial_start = MPI_Wtime();
        find_first_n_primes(n);
        serial_end = MPI_Wtime();
        serial_time = serial_end - serial_start;
        printf("USING SEQUENTIAL APPROACH:\n\n");
        printf("The total time is: %f\n\n\n", serial_time);

        
        printf("The speedUp is: %f\n\n", serial_time/parallel_time);
        printf("The efficienct is: %f\n\n", ((serial_time/parallel_time)/size)*100);
        
        
        
        printf("Prime number %d was given by processor %d\n", 2, rank);
    } else {
        int prime;// This will be the prime number used by this processor
        MPI_Recv(&prime, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
        int number;
        while (1) {
            MPI_Recv(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (number == -1) {
                if (rank < size - 1) {// if this is not the last processor, send the termination tag to hte next one
                    MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                }
                break;
            }
            if (number % prime != 0) { // If the number is prime
                if (rank < size - 1) {  // If this is not the last processor, send this prime to the next processor
                    MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                }
            }
        }
        printf("Prime number %d was given by processor %d\n", prime, rank);
    }
    MPI_Finalize();

    return 0;
}
