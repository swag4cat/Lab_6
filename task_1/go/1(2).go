package main

import (
	"fmt"
	"math/rand"
	"time"
)

func main() {
	var M, N int
	fmt.Print("Введите количество строк (M): ")
	fmt.Scan(&M)
	fmt.Print("Введите количество столбцов (N): ")
	fmt.Scan(&N)

	rand.Seed(time.Now().UnixNano())

	// Создание и инициализация матрицы
	matrix := make([][]int, M)
	frequency := make(map[int]int)

	fmt.Println("\nСгенерированная матрица:")
	for i := 0; i < M; i++ {
		matrix[i] = make([]int, N)
		for j := 0; j < N; j++ {
			matrix[i][j] = rand.Intn(51) + 100 // [100, 150]
			frequency[matrix[i][j]]++
			fmt.Printf("%d\t", matrix[i][j])
		}
		fmt.Println()
	}

	// Поиск наиболее часто встречающегося числа
	mostFrequent := -1
	maxCount := 1 // если все числа уникальны, выводим сообщение
	for num, count := range frequency {
		if count > maxCount {
			maxCount = count
			mostFrequent = num
		}
	}

	if mostFrequent == -1 {
		fmt.Println("\nВсе числа в матрице уникальны.")
	} else {
		fmt.Printf("\nЧисло, которое встречается чаще всего: %d (встречается %d раз)\n", mostFrequent, maxCount)
	}
}
