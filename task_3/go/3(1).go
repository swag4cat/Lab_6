package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
)

// Печать расширенной матрицы
func printMatrix(matrix [][]float64, b []float64) {
	n := len(matrix)
	for i := 0; i < n; i++ {
		for j := 0; j < n; j++ {
			fmt.Printf("%10.5f ", matrix[i][j])
		}
		fmt.Printf("| %10.5f\n", b[i])
	}
	fmt.Println()
}

// Табличный вывод результата
func printSolutionTable(history [][]float64) {
	n := len(history[0])
	fmt.Println()

	// Верхняя граница
	fmt.Print("╔")
	for i := 0; i < n+2; i++ {
		fmt.Print("════════════════╦")
	}
	fmt.Print("\b╗\n") // Удалить последнюю лишнюю черту и поставить угол

	// Заголовок
	fmt.Printf("║%-16s", "       N")
	for i := 0; i < n; i++ {
		fmt.Printf("║%-16s", "       x"+strconv.Itoa(i+1))
	}
	fmt.Printf("║%-15s ║\n", "       εn")

	// Разделитель
	fmt.Print("╠")
	for i := 0; i < n+2; i++ {
		fmt.Print("════════════════╬")
	}
	fmt.Print("\b╣\n")

	// Данные
	for step := 0; step < len(history); step++ {
		fmt.Printf("║%-16s", "       "+strconv.Itoa(step+1))
		maxEps := 0.0
		for i := 0; i < n; i++ {
			fmt.Printf("║   %-13.5f", history[step][i])
			if step > 0 {
				maxEps = math.Max(maxEps, math.Abs(history[step][i]-history[step-1][i]))
			}
		}
		if step == 0 {
			fmt.Printf("║%-16s", "       -")
		} else {
			fmt.Printf("║%-16.5f", maxEps)
		}
		fmt.Println("║")
	}

	// Нижняя граница
	fmt.Print("╚")
	for i := 0; i < n+2; i++ {
		fmt.Print("════════════════╩")
	}
	fmt.Print("\b╝\n")
}

// Метод Гаусса с выбором главного элемента
func gaussWithPivoting(A [][]float64, b []float64, stepsOut *[][]float64) ([]float64, error) {
	n := len(A)
	perm := make([]int, n)
	for i := 0; i < n; i++ {
		perm[i] = i
	}

	// Прямой ход
	for k := 0; k < n; k++ {
		maxRow := k
		maxVal := math.Abs(A[k][k])
		for i := k + 1; i < n; i++ {
			if math.Abs(A[i][k]) > maxVal {
				maxVal = math.Abs(A[i][k])
				maxRow = i
			}
		}

		if maxRow != k {
			A[k], A[maxRow] = A[maxRow], A[k]
			b[k], b[maxRow] = b[maxRow], b[k]
			perm[k], perm[maxRow] = perm[maxRow], perm[k]
		}

		if math.Abs(A[k][k]) < 1e-10 {
			return nil, fmt.Errorf("матрица вырождена или плохо обусловлена")
		}

		div := A[k][k]
		for j := k; j < n; j++ {
			A[k][j] /= div
		}
		b[k] /= div

		for i := k + 1; i < n; i++ {
			factor := A[i][k]
			for j := k; j < n; j++ {
				A[i][j] -= factor * A[k][j]
			}
			b[i] -= factor * b[k]
		}

		fmt.Printf("Шаг %d (после исключения x%d):\n", k+1, k+1)
		printMatrix(A, b)
	}

	// Обратный ход
	x := make([]float64, n)
	for i := n - 1; i >= 0; i-- {
		x[i] = b[i]
		for j := i + 1; j < n; j++ {
			x[i] -= A[i][j] * x[j]
		}
	}

	// Восстановление порядка
	xSorted := make([]float64, n)
	for i := 0; i < n; i++ {
		xSorted[perm[i]] = x[i]
	}

	// Добавим решение в таблицу итераций
	*stepsOut = append(*stepsOut, xSorted)
	return xSorted, nil
}

func main() {
	A := [][]float64{
		{0.87, -0.04, 0.21, -18.0},
		{0.25, -1.23, -0.19, -0.09},
		{-0.21, -0.19, 0.80, -0.13},
		{0.15, -1.31, 0.06, 1.08},
	}

	b := []float64{-1.24, 1.08, 2.56, 0.87}
	var history [][]float64 // Для хранения шагов

	fmt.Println("Исходная система:")
	printMatrix(A, b)

	solution, err := gaussWithPivoting(A, b, &history)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Ошибка: %v\n", err)
		return
	}

	fmt.Println("\nРешение системы (вектор X):")
	for i := 0; i < len(solution); i++ {
		fmt.Printf("x%d = %.6f\n", i+1, solution[i])
	}

	fmt.Println("\nТаблица результата:")
	printSolutionTable(history)
}
