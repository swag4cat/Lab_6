package main

import (
	"fmt"
)

// Функция для подсчёта количества единиц в двоичном представлении числа
func countOnes(num uint64) int {
	count := 0
	for num > 0 {
		count += int(num & 1)
		num >>= 1
	}
	return count
}

func main() {
	var N uint64
	fmt.Print("Ввод: ")
	fmt.Scan(&N)

	bestPair := [2]uint64{0, N}
	maxBananas := countOnes(0) + countOnes(N)

	for a := uint64(1); a <= N/2; a++ {
		b := N - a
		currentBananas := countOnes(a) + countOnes(b)

		if currentBananas > maxBananas {
			maxBananas = currentBananas
			bestPair = [2]uint64{a, b}
		} else if currentBananas == maxBananas {
			// Если количество бананов одинаковое, выбираем пару с большей разностью
			if (b - a) > (bestPair[1] - bestPair[0]) {
				bestPair = [2]uint64{a, b}
			}
		}
	}

	fmt.Printf("Вывод: %d %d\n", bestPair[0], bestPair[1])
}
