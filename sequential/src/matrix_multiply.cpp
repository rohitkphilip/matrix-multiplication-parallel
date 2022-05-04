#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

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

int main(int argc, char *argv[]) {
    int i, j, k;

    // Set params from args
    istringstream iss(argv[1]);
    int size;
    if (iss >> size) {
        string path = argv[2];
        int** matLeft = getMatFromFile(path + "/Left/" + to_string(size) + ".txt", size);
        int** matRight = getMatFromFile(path + "/Right/" + to_string(size) + ".txt", size);
        
        // Start Timer
        long long int startTime = clock();

        // Print Matrices
        cout << "A:" << endl;
        printPretty(matLeft, size);
        cout << "B:" << endl;
        printPretty(matRight, size);

        int** ans = (int**) malloc (sizeof(int*) * size);
        for (int i = 0; i < size; i++) {
            ans[i] = (int*) malloc (sizeof(int) * size);
        }

        // Multiply
        for(k = 0; k < size; k ++){
            for(i = 0; i < size; i ++){
                ans[i][k] = 0;
                for(j = 0; j < size; j ++){
                    ans[i][k] += matLeft[i][j] * matRight[j][k];
                }
            }
        }

        std::cout << "ans: " << std::endl;
        printPretty(ans, size);

        // End Timer
        long long int endTime = clock();
        double diff = (double)((double)(endTime - startTime) / 1000000);
        cout << fixed << "Time " << diff << "s" << endl;

    } else {
        cout << "Invalid Arguments" << endl;
    }
    return 0;
}
