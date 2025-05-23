package aes

import (
	"encoding/hex"
	"fmt"
	"os"
)

// Сохраняет данные в файл
func SaveToFile(filename string, data []byte) {
	err := os.WriteFile(filename, data, 0644)
	if err != nil {
		fmt.Printf("Ошибка записи файла %s: %v\n", filename, err)
	}
}

// Загружает данные из файла
func LoadFromFile(filename string) ([]byte, error) {
	return os.ReadFile(filename)
}

// Выводит данные в hex с меткой
func PrintHex(label string, data []byte) {
	fmt.Printf("%s (%d байт): %s\n", label, len(data), hex.EncodeToString(data))
}
