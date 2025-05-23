package aes

// Преобразует Block в []byte (16 байт)
func BlockToBytes(block Block) []byte {
	buf := make([]byte, 16)

	// Итерируем по столбцам
	for col := 0; col < 4; col++ {
		for row := 0; row < 4; row++ {
			buf[col*4+row] = block[row][col]
		}
	}
	return buf
}

// Преобразует []byte (16 байт) в Block
func BytesToBlock(buf []byte) Block {
	var block Block

	// Итерируем по столбцам
	for col := 0; col < 4; col++ {
		for row := 0; row < 4; row++ {
			block[row][col] = buf[col*4+row]
		}
	}
	return block
}
