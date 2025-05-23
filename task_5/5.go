package main

import (
	"fmt"
	"math/rand"
	"time"
)

// Функция для генерации случайной доски
func generateRandomBoard(n, m int) [][]bool {
	rand.Seed(time.Now().UnixNano())
	board := make([][]bool, n)
	for i := range board {
		board[i] = make([]bool, m)
		for j := range board[i] {
			board[i][j] = rand.Intn(2) == 1
		}
	}
	return board
}

// Функция для печати доски
func printBoard(board [][]bool) {
	for _, row := range board {
		for _, cell := range row {
			if cell {
				fmt.Print("H ")
			} else {
				fmt.Print("T ")
			}
		}
		fmt.Println()
	}
}

// Функция для проверки, является ли доска шахматной с заданным первым значением
func isChessPattern(board [][]bool, firstValue bool) bool {
	expected := firstValue
	for _, row := range board {
		currentRowExpected := expected
		for _, cell := range row {
			if cell != currentRowExpected {
				return false
			}
			currentRowExpected = !currentRowExpected
		}
		expected = !expected
	}
	return true
}

// Функция для переворота двух соседних монет с выводом шага
func flipCoins(board [][]bool, i1, j1, i2, j2 int, stepNumber int) [][]bool {
	fmt.Printf("Шаг %d: переворот (%d, %d) и (%d, %d)\n", stepNumber, i1, j1, i2, j2)
	board[i1][j1] = !board[i1][j1]
	board[i2][j2] = !board[i2][j2]
	printBoard(board)
	fmt.Println("----------------------------------")
	return board
}

// Функция для поиска минимального количества ходов с выводом шагов
func findMinMoves(board [][]bool, firstValue bool) int {
	moves := 0
	step := 1
	n := len(board)
	if n == 0 {
		return 0
	}
	m := len(board[0])

	// Создаем копию доски, чтобы не изменять оригинал
	boardCopy := make([][]bool, n)
	for i := range board {
		boardCopy[i] = make([]bool, m)
		copy(boardCopy[i], board[i])
	}

	for i := 0; i < n; i++ {
		for j := 0; j < m; j++ {
			expected := (i+j)%2 == 0
			if firstValue {
				expected = (i+j)%2 == 0
			} else {
				expected = (i+j)%2 != 0
			}
			if boardCopy[i][j] != expected {
				if j+1 < m {
					boardCopy = flipCoins(boardCopy, i, j, i, j+1, step)
					step++
					moves++
				} else if i+1 < n {
					boardCopy = flipCoins(boardCopy, i, j, i+1, j, step)
					step++
					moves++
				} else {
					return 1<<31 - 1 // INT_MAX
				}
			}
		}
	}

	// Проверяем, получили ли мы нужный узор
	if isChessPattern(boardCopy, firstValue) {
		return moves
	}
	return 1<<31 - 1
}

func main() {
	var n, m int
	fmt.Print("Введите размеры доски N и M: ")
	fmt.Scan(&n, &m)

	// Генерация случайной доски
	board := generateRandomBoard(n, m)

	fmt.Println("Исходная доска:")
	printBoard(board)
	fmt.Println("----------------------------------")

	// Проверяем оба возможных шахматных узора
	moves1 := findMinMoves(board, true)  // Первый вариант: начинается с H
	moves2 := findMinMoves(board, false) // Второй вариант: начинается с T

	if moves1 == 1<<31-1 && moves2 == 1<<31-1 {
		fmt.Println("Невозможно привести доску к шахматному узору.")
	} else {
		minMoves := moves1
		if moves2 < moves1 {
			minMoves = moves2
		}
		fmt.Printf("Минимальное количество ходов: %d\n", minMoves)

		// Выводим, какой узор получится
		if moves1 < moves2 {
			fmt.Println("Получится узор, начинающийся с H в левом верхнем углу")
		} else {
			fmt.Println("Получится узор, начинающийся с T в левом верхнем углу")
		}
	}
}
