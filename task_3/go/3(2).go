package main

import (
	"fmt"
	"math"
	"strconv"
)

const (
	EPS      = 1e-3
	MAX_ITER = 15
)

// Проверка диагонального преобладания
func checkDiagonalDominance(A [][]float64) bool {
	n := len(A)
	for i := 0; i < n; i++ {
		diag := math.Abs(A[i][i])
		sum := 0.0
		for j := 0; j < n; j++ {
			if i != j {
				sum += math.Abs(A[i][j])
			}
		}
		if diag <= sum {
			return false
		}
	}
	return true
}

// Проверка положительной определённости (упрощённый критерий Сильвестра)
func isPositiveDefinite(A [][]float64) bool {
	n := len(A)
	for k := 1; k <= n; k++ {
		det := 1.0
		for i := 0; i < k; i++ {
			det *= A[i][i]
		}
		if det <= 0 {
			return false
		}
	}
	return true
}

// Печать таблицы итераций
func printSolutionTable(history [][]float64) {
	n := len(history[0])
	fmt.Println()

	// Верхняя граница
	fmt.Print("╔")
	for i := 0; i < n+2; i++ {
		fmt.Print("════════════════╦")
	}
	fmt.Print("\b╗\n")

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

// Преобразование к каноническому виду: x = Cx + f
func transformToCanonical(A [][]float64, b []float64) (C [][]float64, f []float64) {
	n := len(A)
	C = make([][]float64, n)
	f = make([]float64, n)

	for i := 0; i < n; i++ {
		C[i] = make([]float64, n)
		diag := A[i][i]
		f[i] = b[i] / diag
		for j := 0; j < n; j++ {
			if i == j {
				C[i][j] = 0
			} else {
				C[i][j] = -A[i][j] / diag
			}
		}
	}
	return C, f
}

// Вывод матрицы C и вектора f
func printCanonicalForm(C [][]float64, f []float64) {
	fmt.Println("\nМатрица C (коэффициенты итераций):")
	for _, row := range C {
		for _, val := range row {
			fmt.Printf("%12.6f", val)
		}
		fmt.Println()
	}

	fmt.Println("\nВектор f (свободные члены):")
	for _, val := range f {
		fmt.Printf("%12.6f\n", val)
	}
}

// Метод Зейделя
func seidelMethod(A [][]float64, b []float64, history *[][]float64) ([]float64, int) {
	n := len(A)
	x := make([]float64, n)
	prevX := make([]float64, n)
	iterations := 0

	for iter := 0; iter < MAX_ITER; iter++ {
		for i := 0; i < n; i++ {
			sum := b[i]
			for j := 0; j < n; j++ {
				if j < i {
					sum -= A[i][j] * x[j]
				} else if j > i {
					sum -= A[i][j] * prevX[j]
				}
			}
			x[i] = sum / A[i][i]
		}

		// Добавляем копию текущего решения в историю
		xCopy := make([]float64, n)
		copy(xCopy, x)
		*history = append(*history, xCopy)

		// Проверка сходимости
		maxDelta := 0.0
		for i := 0; i < n; i++ {
			maxDelta = math.Max(maxDelta, math.Abs(x[i]-prevX[i]))
		}

		if maxDelta < EPS {
			iterations = iter + 1
			break
		}

		copy(prevX, x)
	}

	return x, iterations
}

func main() {
	A := [][]float64{
		{0.87, -0.04, 0.21, -18.0},
		{0.25, -1.23, -0.19, -0.09},
		{-0.21, -0.19, 0.80, -0.13},
		{0.15, -1.31, 0.06, 1.08},
	}

	b := []float64{-1.24, 1.08, 2.56, 0.87}

	// Проверка условий
	if checkDiagonalDominance(A) {
		fmt.Println("Выполняется диагональное преобладание.")
	} else {
		fmt.Println("Предупреждение: диагональное преобладание не выполняется. Метод может не сойтись.")
	}

	if isPositiveDefinite(A) {
		fmt.Println("Матрица A положительно определённая.")
	} else {
		fmt.Println("Матрица A не является положительно определённой.")
	}

	// Преобразование
	C, f := transformToCanonical(A, b)
	printCanonicalForm(C, f)

	// Итерации
	var history [][]float64
	solution, iterations := seidelMethod(A, b, &history)

	// Вывод
	fmt.Println("\nРешение системы (метод Зейделя):")
	for i := 0; i < len(solution); i++ {
		fmt.Printf("x%d = %.6f\n", i+1, solution[i])
	}

	fmt.Println("\nТаблица итераций:")
	printSolutionTable(history)

	// Расчёт количества операций
	n := len(A)
	if iterations > 0 {
		operations := 2 * n * n * iterations
		fmt.Printf("\nКоличество операций (по формуле 2n^2 * k): %d\n", operations)
	} else {
		fmt.Println("\nМетод не сошёлся — количество операций не рассчитывается.")
	}
}
