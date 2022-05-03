package main

import (
	"log"
	"math/rand"
	"os"
	"strconv"
)

func generateMatrix(row int, col int) []int {
	mat := make([]int, row*col)
	for i := 0; i < row*col; i++ {
		mat[i] = rand.Intn(100)
	}
	return mat
}

func exportMatrixToFile(mat []int, path string, col int) {
	var matStr string

	for i, num := range mat {
		matStr += strconv.Itoa(num) + " "
		if i%col == col-1 {
			matStr += "\n"
		}
	}

	f, err := os.OpenFile(path, os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatal(err)
	}
	f.WriteString(matStr[:len(matStr)-1])
}

func main() {
	sizes := []int{3, 30, 300, 1000}
	for _, size := range sizes {
		exportMatrixToFile(generateMatrix(size, size), "../Matrices/Left/"+strconv.Itoa(size)+".txt", size)
		exportMatrixToFile(generateMatrix(size, size), "../Matrices/Right/"+strconv.Itoa(size)+".txt", size)
	}
}
