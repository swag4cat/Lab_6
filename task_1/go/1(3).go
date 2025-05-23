package main

import (
	"fmt"
	"math/rand"
	"os"
	"os/exec"
	"runtime"
	"time"
)

const (
	M           = 20       // Строки
	N           = 40       // Столбцы
	GENERATIONS = 100      // Количество поколений
	DELAY       = 200      // Задержка в миллисекундах
	NUM_GLIDERS = 5        // Количество глайдеров
)

// Очистка экрана
func clearScreen() {
	var cmd *exec.Cmd
	if runtime.GOOS == "windows" {
		cmd = exec.Command("cmd", "/c", "cls")
	} else {
		cmd = exec.Command("clear")
	}
	cmd.Stdout = os.Stdout
	cmd.Run()
}

// Вывод матрицы
func printGrid(grid [][]int) {
	clearScreen()
	for i := 0; i < M; i++ {
		for j := 0; j < N; j++ {
			if grid[i][j] == 1 {
				fmt.Print("█")
			} else {
				fmt.Print(" ")
			}
		}
		fmt.Println()
	}
}

// Подсчёт живых соседей (с учётом замкнутой плоскости)
func countLiveNeighbors(grid [][]int, x, y int) int {
	count := 0
	for dx := -1; dx <= 1; dx++ {
		for dy := -1; dy <= 1; dy++ {
			if dx == 0 && dy == 0 {
				continue
			}
			nx := (x + dx + M) % M
			ny := (y + dy + N) % N
			count += grid[nx][ny]
		}
	}
	return count
}

// Обновление матрицы по правилам Конвея
func nextGeneration(grid [][]int) [][]int {
	newGrid := make([][]int, M)
	for i := range newGrid {
		newGrid[i] = make([]int, N)
		copy(newGrid[i], grid[i])
	}

	for i := 0; i < M; i++ {
		for j := 0; j < N; j++ {
			neighbors := countLiveNeighbors(grid, i, j)
			if grid[i][j] == 1 {
				if neighbors == 2 || neighbors == 3 {
					newGrid[i][j] = 1
				} else {
					newGrid[i][j] = 0
				}
			} else {
				if neighbors == 3 {
					newGrid[i][j] = 1
				} else {
					newGrid[i][j] = 0
				}
			}
		}
	}
	return newGrid
}

// Инициализация Glider'а
func placeGlider(grid [][]int, x, y int) {
	grid[x][y+1] = 1
	grid[x+1][y+2] = 1
	grid[x+2][y] = 1
	grid[x+2][y+1] = 1
	grid[x+2][y+2] = 1
}

func main() {
	rand.Seed(time.Now().UnixNano())

	// Инициализация сетки
	grid := make([][]int, M)
	for i := range grid {
		grid[i] = make([]int, N)
	}

	// Размещение глайдеров
	for i := 0; i < NUM_GLIDERS; i++ {
		// Глайдер занимает 3x3 область, поэтому ограничим координаты
		x := rand.Intn(M - 3)
		y := rand.Intn(N - 3)
		placeGlider(grid, x, y)
	}

	// Запуск симуляции
	for gen := 0; gen < GENERATIONS; gen++ {
		printGrid(grid)
		grid = nextGeneration(grid)
		time.Sleep(DELAY * time.Millisecond)
	}
}
