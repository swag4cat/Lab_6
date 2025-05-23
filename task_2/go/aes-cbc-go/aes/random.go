package aes

import (
	"crypto/rand"
)

// Типы
type Byte = uint8
type Block [4][4]Byte

// Генерирует случайный ключ заданной длины (по умолчанию 16 байт)
func GenerateRandomKey(length int) []Byte {
	key := make([]Byte, length)
	_, err := rand.Read(key)
	if err != nil {
		panic("Ошибка генерации ключа: " + err.Error())
	}
	return key
}

// Генерирует случайный вектор инициализации (IV)
func GenerateRandomIV() Block {
	var iv Block
	buf := make([]byte, 16)
	_, err := rand.Read(buf)
	if err != nil {
		panic("Ошибка генерации IV: " + err.Error())
	}
	for col := 0; col < 4; col++ {
		for row := 0; row < 4; row++ {
			iv[row][col] = Byte(buf[col*4+row])
		}
	}
	return iv
}
