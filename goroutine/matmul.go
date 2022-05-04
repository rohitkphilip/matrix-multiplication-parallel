package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"
	"time"
)

type Response struct {
	start  int
	rows   int
	values []int
}

func master(matLeft []int, matRight []int, size int, numWorkers int, ans chan []int) {

	// Print Matrices
	fmt.Println("A:")
	printPretty(matLeft, size)
	fmt.Println("B:")
	printPretty(matRight, size)

	rowsPerRoutine := size / numWorkers
	excessRows := size % numWorkers
	start := 0
	response := make(chan Response)

	// Split Work
	for i := 1; i <= numWorkers; i++ {
		end := start + rowsPerRoutine
		if i <= excessRows {
			end += 1
		}

		go worker(
			start,
			end-start,
			size,
			matLeft[size*start:size*end],
			matRight,
			response,
		)

		start = end
	}

	matAns := make([]int, size*size)

	// Gather Work
	for i := 0; i < numWorkers; i++ {
		res := <-response
		startIndex := res.start * size
		endIndex := startIndex + (res.rows * size)
		copy(matAns[startIndex:endIndex], res.values)
	}

	ans <- matAns
}

func worker(start int, numRows int, numCols int, rows []int, cols []int, response chan Response) {
	ans := make([]int, numCols)

	for k := 0; k < numCols; k++ {
		for i := 0; i < numRows; i++ {
			ans[(i*numCols)+k] = 0
			for j := 0; j < numCols; j++ {
				ans[(i*numCols)+k] += rows[(i*numCols)+j] * cols[(j*numCols)+k]
			}
		}
	}
	response <- Response{
		start,
		numRows,
		ans,
	}
}

func getMatFromFile(path string, size int) []int {
	data, err := os.ReadFile(path + strconv.Itoa(size) + ".txt")
	if err != nil {
		panic(err)
	}
	values := string(data)
	lines := strings.Split(values, "\n")
	mat := make([]int, size*size)
	for i, line := range lines {
		numStrs := strings.Split(line, " ")
		for j, numStr := range numStrs {
			num, err2 := strconv.Atoi(numStr)
			if err2 == nil {
				mat[(i*size)+j] = num
			}
		}
	}
	return mat
}

func printPretty(mat []int, col int) {
	var matStr string

	for i, num := range mat {
		matStr += strconv.Itoa(num) + " "
		if i%col == col-1 {
			matStr += "\n"
		}
	}

	fmt.Println(matStr)
}

func main() {
	args := os.Args[1:]
	numWorkers, err := strconv.Atoi(args[0])
	if err != nil {
		panic(err)
	}
	numWorkers -= 1
	size, err2 := strconv.Atoi(args[1])
	if err2 != nil {
		panic(err2)
	}
	dir := args[2]

	matLeft := getMatFromFile(dir+"/Left/", size)
	matRight := getMatFromFile(dir+"/Right/", size)

	// Start Timer
	startTime := time.Now()

	ans := make(chan []int)
	go master(matLeft, matRight, size, numWorkers, ans)

	fmt.Println("ans:")
	printPretty(<-ans, size)

	// End Timer
	fmt.Printf("Time %vs\n", time.Since(startTime).Seconds())
}
