package aes

import (

	"fmt"
	"os"
)

// Выводит ключ в консоль
func PrintKey(key []Byte) {
	fmt.Print("Ключ (16 байт): ")
	for _, b := range key {
		fmt.Printf("%02x ", b)
	}
	fmt.Println()
}

// Выводит блок в консоль с заголовком
func PrintBlock(block Block, title string) {
	if title != "" {
		fmt.Println(title)
	}
	for row := 0; row < 4; row++ {
		for col := 0; col < 4; col++ {
			fmt.Printf("%02x ", block[row][col])
		}
		fmt.Println()
	}
}

// Сохраняет ключ в бинарный файл
func SaveKeyToFile(filename string, key []Byte) {
	err := os.WriteFile(filename, key, 0644)
	if err != nil {
		fmt.Printf("Ошибка записи ключа в файл %s: %v\n", filename, err)
	}
}
