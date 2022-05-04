#include <iostream>



#define MATRIX_SIZE 1000

int mat1[MATRIX_SIZE][MATRIX_SIZE];
int mat2[MATRIX_SIZE][MATRIX_SIZE];
int result_mat[MATRIX_SIZE][MATRIX_SIZE];

template<int rows, int cols> 
void FillMatrix(int (&matrix)[rows][cols]){
	for(int i = 0; i < cols; i ++){
        for(int j = 0; j < rows; j ++){
        //   matrix[i][j] = GenerateRandomNumber();
        // matrix[i][j] = std::rand();
        matrix[i][j] = 1;
        }
    }
}

template<int rows, int cols> 
void FillMatrix2(int (&matrix)[rows][cols]){
	for(int i = 0; i < cols; i ++){
        for(int j = 0; j < rows; j ++){
        //   matrix[i][j] = GenerateRandomNumber();
        matrix[i][j] = std::rand();
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

int main()
{
    int i, j, k;

    std::cout << std::endl << "Generating matrix A with size " << MATRIX_SIZE << "x" << MATRIX_SIZE  << std::endl;
	FillMatrix(mat1);
	PrintMatrix(mat1);

    std::cout << std::endl << "Generating matrix B with size " << MATRIX_SIZE << "x" << MATRIX_SIZE << std::endl;
	FillMatrix(mat2);
	PrintMatrix(mat2);

    std::cout << std::endl << "Starting multiplication ... " << std::endl;
	clock_t time = clock();

	for(k = 0; k < MATRIX_SIZE; k ++){
		for(i = 0; i < MATRIX_SIZE; i ++){
			result_mat[i][k] = 0;
			for(j = 0; j < MATRIX_SIZE; j ++){
				result_mat[i][k] += mat1[i][j] * mat2[j][k];
			}
		}
	}
    std::cout << std::endl << "Multiplication Result: " << std::endl;
	PrintMatrix(result_mat);
	time = clock() - time;
	double diff = (double)(time / (1.0 * 1000000));

	
    std::cout << std::endl << MATRIX_SIZE << "x" << MATRIX_SIZE << " - " << diff << " seconds" << std::endl;

    return 0;
}
