package main

import (
	"fmt"
	"math/rand"
	"time"
)

func main() {
	var N int
	fmt.Print("Введите порядок квадратной матрицы N: ")
	fmt.Scan(&N)

	// Инициализация генератора случайных чисел
	rand.Seed(time.Now().UnixNano())

	// Создание и инициализация матрицы
	matrix := make([][]int, N)
	fmt.Println("\nСгенерированная матрица:")
	for i := 0; i < N; i++ {
		matrix[i] = make([]int, N)
		for j := 0; j < N; j++ {
			matrix[i][j] = rand.Intn(201) - 100 // от -100 до 100
			fmt.Printf("%d\t", matrix[i][j])
		}
		fmt.Println()
	}

	// Поиск строки с наибольшим числом положительных элементов
	maxPositives := 0
	maxRowIndex := 0

	for i := 0; i < N; i++ {
		positiveCount := 0
		for j := 0; j < N; j++ {
			if matrix[i][j] > 0 {
				positiveCount++
			}
		}
		if positiveCount > maxPositives {
			maxPositives = positiveCount
			maxRowIndex = i
		}
	}

	// Инициализация нового массива найденной строкой
	resultRow := matrix[maxRowIndex]

	// Вывод результата
	fmt.Printf("\nСтрока с наибольшим количеством положительных чисел (номер %d):\n", maxRowIndex)
	for _, val := range resultRow {
		fmt.Printf("%d ", val)
	}
	fmt.Println()
}
