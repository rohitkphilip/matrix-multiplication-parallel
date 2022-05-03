package main

import (
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
	"time"
)

type Response struct {
	start  int
	values []int
}

func master(matLeft []int, matRight []int, rows int, cols int, numWorkers int, ans chan []int) {
	rowsPerRoutine := rows / numWorkers
	excessRows := rows % numWorkers
	start := 0
	response := make(chan Response)
	for i := 1; i <= numWorkers; i++ {
		end := start + rowsPerRoutine
		if i <= excessRows {
			end += 1
		}
		go work(
			start,
			end-start,
			cols,
			matLeft[cols*start:cols*end],
			matRight,
			response,
		)
		start = end
	}
	responses := make([]Response, numWorkers)
	for i := 0; i < numWorkers; i++ {
		responses[i] = <-response
		// fmt.Println(responses[i])
	}
	sort.Slice(responses, func(i, j int) bool {
		return responses[i].start < responses[j].start
	})
	slices := []int{}
	for _, response := range responses {
		slices = append(slices, response.values...)
	}
	ans <- slices
}

func work(start int, numRows int, numCols int, rows []int, cols []int, response chan Response) {
	// fmt.Println("work started!")
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
		ans,
	}
	// fmt.Println("work done!")
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
				// 	panic(err2)
				// }
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
	start := time.Now()
	args := os.Args[1:]
	numWorkers, err := strconv.Atoi(args[0])
	if err != nil {
		panic(err)
	}
	size, err2 := strconv.Atoi(args[1])
	if err2 != nil {
		panic(err2)
	}
	dir := args[2]
	matLeft := getMatFromFile(dir+"/Left/", size)
	matRight := getMatFromFile(dir+"/Right/", size)
	fmt.Println("A:")
	printPretty(matLeft, size)
	fmt.Println("B:")
	printPretty(matRight, size)
	ans := make(chan []int)
	go master(matLeft, matRight, size, size, numWorkers, ans)
	fmt.Println("ans:")
	printPretty(<-ans, size)
	fmt.Printf("Time %v\n", time.Since(start))
}
