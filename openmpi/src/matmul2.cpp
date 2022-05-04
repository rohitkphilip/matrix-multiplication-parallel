#include <stdio.h>
#include <iostream>
#include <mpi.h>
#include <algorithm>
#include <ctime>


#define MASTER_RANK 0

#define MASTER_TAG 1
#define WORKER_TAG 2

#define MATRIX_SIZE 4000


#define NOT_ENOUGH_PROCESSES_NUM_ERROR 1

MPI_Status status;

int mat1[MATRIX_SIZE][MATRIX_SIZE];
int mat2[MATRIX_SIZE][MATRIX_SIZE];
int result_mat[MATRIX_SIZE][MATRIX_SIZE];

// int GenerateRandomNumber(){
// 	return std::rand() % 9 + 1;
// }

template<int rows, int cols> 
void FillMatrix(int (&matrix)[rows][cols]){
	for(int i = 0; i < cols; i ++){
        for(int j = 0; j < rows; j ++){
        //   matrix[i][j] = GenerateRandomNumber();
        matrix[i][j] = std::rand();
        matrix[i][j] = 1;
        }
    }
}

template<int rows, int cols> 
void PrintMatrix(int (&matrix)[rows][cols]){
    std::cout << std::endl;
	for(int i = 0; i < rows; i ++){
		for(int j = 0; j < cols; j ++){
            std::cout << matrix[i][j] << " ";
		}
        std::cout << std::endl;
	}
}


int main(int argc, char *argv[])
{
	int comm_size;
	int process_rank;
	int process_id;
	int offset;
	int rows_num;
	int workers_num;
	int remainder;
	int whole_part;
	int message_tag;
	int i;
	int j;
	int k;


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

	if(comm_size < 2){
		MPI_Abort(MPI_COMM_WORLD, NOT_ENOUGH_PROCESSES_NUM_ERROR);
	}

	if(process_rank == MASTER_RANK){
        std::cout << "Generating matrices" << std::endl;
		
        std::cout << std::endl << "Generating matrix A with size " << MATRIX_SIZE << "x" << MATRIX_SIZE  << std::endl;
		FillMatrix(mat1);
		PrintMatrix(mat1);

        std::cout << std::endl << "Generating matrix B with size " << MATRIX_SIZE << "x" << MATRIX_SIZE << std::endl;
		FillMatrix(mat2);
		PrintMatrix(mat2);

        std::cout << std::endl << "Starting multiplication ... " << std::endl;
		clock_t time = clock();

		workers_num = comm_size - 1;
		whole_part = MATRIX_SIZE / workers_num;
		remainder = MATRIX_SIZE % workers_num;
		offset = 0;

		message_tag = MASTER_TAG;
		for(process_id = 1; process_id <= workers_num; process_id ++ ){
			rows_num = process_id <= remainder ? whole_part + 1 : whole_part;
			MPI_Send(&offset, 1, MPI_INT, process_id, message_tag, MPI_COMM_WORLD);
			MPI_Send(&rows_num, 1, MPI_INT, process_id, message_tag, MPI_COMM_WORLD);
			MPI_Send(&mat1[offset][0], rows_num * MATRIX_SIZE, MPI_INT, process_id, message_tag, MPI_COMM_WORLD);
			MPI_Send(&mat2, MATRIX_SIZE * MATRIX_SIZE, MPI_INT, process_id, message_tag, MPI_COMM_WORLD);

			offset += rows_num;
		}

		message_tag = WORKER_TAG;
		for(process_id = 1; process_id <= workers_num; process_id ++){
			MPI_Recv(&offset, 1, MPI_INT, process_id, message_tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&rows_num, 1, MPI_INT, process_id, message_tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&result_mat[offset][0], rows_num * MATRIX_SIZE, MPI_INT, process_id, message_tag, MPI_COMM_WORLD, &status);
		}
		// printf("\nResult A*B");
        std::cout << std::endl << "Multiplication Result: " << std::endl;
		PrintMatrix(result_mat);
		time = clock() - time;
		double diff = (double)(time / (1.0 * 1000000));

		// printf("\n%dx%d - %f seconds\n", MATRIX_SIZE, MATRIX_SIZE, diff);
        std::cout << std::endl << MATRIX_SIZE << "x" << MATRIX_SIZE << " - " << diff << " seconds" << std::endl;
	} 
	
	if(process_rank != MASTER_RANK){
		message_tag = MASTER_TAG;
		MPI_Recv(&offset, 1, MPI_INT, MASTER_RANK, message_tag, MPI_COMM_WORLD, &status);
		MPI_Recv(&rows_num, 1, MPI_INT, MASTER_RANK, message_tag, MPI_COMM_WORLD, &status);
		MPI_Recv(&mat1, rows_num * MATRIX_SIZE, MPI_INT, MASTER_RANK, message_tag, MPI_COMM_WORLD, &status);
		MPI_Recv(&mat2, MATRIX_SIZE * MATRIX_SIZE, MPI_INT, MASTER_RANK, message_tag, MPI_COMM_WORLD, &status);

		for(k = 0; k < MATRIX_SIZE; k ++){
			for(i = 0; i < rows_num; i ++){
				result_mat[i][k] = 0;
				for(j = 0; j < MATRIX_SIZE; j ++){
					result_mat[i][k] += mat1[i][j] * mat2[j][k];
				}
			}
		}

		message_tag = WORKER_TAG;
		MPI_Send(&offset, 1, MPI_INT, MASTER_RANK, message_tag, MPI_COMM_WORLD);
		MPI_Send(&rows_num, 1, MPI_INT, MASTER_RANK, message_tag, MPI_COMM_WORLD);
		MPI_Send(&result_mat, rows_num * MATRIX_SIZE, MPI_INT, MASTER_RANK, message_tag, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}