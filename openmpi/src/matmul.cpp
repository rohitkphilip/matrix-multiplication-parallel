#include <mpi.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

#define MASTER_TAG 1
#define WORKER_TAG 2

MPI_Status status;

int** getMatFromFile(string path, int size) {
    ifstream file(path);
    string line;
    int** mat = (int**) malloc(sizeof(int*) * size);
    for (int i = 0; i < size; i++) {
        mat[i] = (int*) malloc(sizeof(int) * size);
        getline(file, line);
        istringstream iss(line);
        for (int j = 0; j < size; j++) {
            iss >> mat[i][j];
        }
    }
    return mat;
}

void printPretty(int** mat, int size){
    cout << endl;
	for(int i = 0; i < size; i ++){
		for(int j = 0; j < size; j ++){
			cout << mat[i][j] << " ";
		}
        cout << endl;
	}
    cout << endl;
}

void master(int** matLeft, int** matRight, int size, int numWorkers) {
    // Start Timer
    long long int startTime = clock();

    // Print Matrices
    cout << "A:" << endl;
    printPretty(matLeft, size);
    cout << "B:" << endl;
    printPretty(matRight, size);

    int rowsPerProcess = size / numWorkers;
    int excessRows = size % numWorkers;
    int start = 0;
    int rows;

    // Split Work
    for (int i = 1; i <= numWorkers; i++) {
        rows = rowsPerProcess;
        if (i < excessRows) rows += 1;
        MPI_Send(&start, 1, MPI_INT, i, MASTER_TAG, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, i, MASTER_TAG, MPI_COMM_WORLD);

        start = start + rows;
    }

    int** ans = (int**) malloc (sizeof(int*) * size);
    for (int i = 0; i < size; i++) {
        ans[i] = (int*) malloc (sizeof(int) * size);
    }

    // Gather Work
    for (int i = 1; i <= numWorkers; i++) {
        MPI_Recv(&start, 1, MPI_INT, i, WORKER_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, i, WORKER_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&ans[start][0], rows * size, MPI_INT, i, WORKER_TAG, MPI_COMM_WORLD, &status);
    }

    cout << "ans:" << endl;
    printPretty(ans, size);

    // End Timer
    long long int endTime = clock();
    double diff = (double)((double)(endTime - startTime) / 1000000);
    cout << fixed << "Time " << diff << "s" << endl;
}


void worker(int** matLeft, int** matRight, int size) {
    int start;
    int rows;

    // Receive Work
    MPI_Recv(&start, 1, MPI_INT, 0, MASTER_TAG, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI_INT, 0, MASTER_TAG, MPI_COMM_WORLD, &status);

    int ans[rows][size];

    for(int k = 0; k < size; k++){
        for(int i = 0; i < rows; i++){
            ans[i][k] = 0;
            for(int j = 0; j < size; j++){
                ans[i][k] += matLeft[start + i][j] * matRight[j][k];
            }
        }
    }
    MPI_Send(&start, 1, MPI_INT, 0, WORKER_TAG, MPI_COMM_WORLD);
    MPI_Send(&rows, 1, MPI_INT, 0, WORKER_TAG, MPI_COMM_WORLD);
	MPI_Send(&ans, rows * size, MPI_INT, 0, WORKER_TAG, MPI_COMM_WORLD);
}

int main(int argc, char *argv[]) {
    int numWorkers;
    int processID;
    
    // Set MPI params
    MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numWorkers);
	MPI_Comm_rank(MPI_COMM_WORLD, &processID);
    numWorkers -= 1;

    // Set params from args
    istringstream iss(argv[1]);
    int size;
    if (iss >> size) {
        string path = argv[2];
        int** matLeft = getMatFromFile(path + "/Left/" + to_string(size) + ".txt", size);
        int** matRight = getMatFromFile(path + "/Right/" + to_string(size) + ".txt", size);
        
        if (processID == 0) {
            master(matLeft, matRight, size, numWorkers);
        } else {
            worker(matLeft, matRight, size);
        }
    } else {
        cout << "Invalid Arguments" << endl;
    }
    return 0;
}
