package aes

import (
	"strings"
)

// TextToBlocks — переводит строку в блоки AES (с выравниванием пробелами)
func TextToBlocks(text string) []Block {
	// Дополняем пробелами до кратности 16
	for len(text)%16 != 0 {
		text += " "
	}

	blocks := []Block{}
	for i := 0; i < len(text); i += 16 {
		var block Block
		chunk := text[i : i+16] // Берем кусок строки длиной 16 символов

		// Заполняем блок по столбцам
		for col := 0; col < 4; col++ {
			for row := 0; row < 4; row++ {
				block[row][col] = Byte(chunk[col*4+row])
			}
		}
		blocks = append(blocks, block)
	}

	return blocks
}

// BlocksToText — переводит блоки обратно в строку и удаляет паддинг
func BlocksToText(blocks []Block) string {
	var sb strings.Builder

	// Собираем строку из блоков (обратное преобразование)
	for _, block := range blocks {
		// Читаем данные из блока
		for col := 0; col < 4; col++ {
			for row := 0; row < 4; row++ {
				sb.WriteByte(block[row][col])
			}
		}
	}

	// Удаление завершающих пробелов
	return strings.TrimRight(sb.String(), " ")
}

// XorBlocks — поэлементный XOR двух блоков
func XorBlocks(a, b Block) Block {
	var result Block
	for row := 0; row < 4; row++ {
		for col := 0; col < 4; col++ {
			result[row][col] = a[row][col] ^ b[row][col]
		}
	}
	return result
}
