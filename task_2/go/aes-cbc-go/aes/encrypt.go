package aes

// Шифрует один блок с помощью раундовых ключей
func EncryptBlock(input Block, roundKeys []Block) Block {
	state := XorBlocks(input, roundKeys[0]) // Начальный AddRoundKey

	// Основные раунды
	for round := 1; round < 10; round++ {
		subBytes(&state)
		shiftRows(&state)
		mixColumns(&state)
		state = XorBlocks(state, roundKeys[round])
	}

	// Финальный раунд (без MixColumns)
	subBytes(&state)
	shiftRows(&state)
	state = XorBlocks(state, roundKeys[10])

	return state
}

// Подстановка байтов по sbox
func subBytes(state *Block) {
	for row := 0; row < 4; row++ {
		for col := 0; col < 4; col++ {
			state[row][col] = sbox[state[row][col]]
		}
	}
}

// Циклический сдвиг строк
func shiftRows(state *Block) {
	tmp := *state
	for i := 1; i < 4; i++ {
		for j := 0; j < 4; j++ {
			state[i][j] = tmp[i][(j+i)%4] // Сдвиг влево
		}
	}
}

// Смешивание столбцов
func mixColumns(state *Block) {
	for col := 0; col < 4; col++ {
		s0 := state[0][col]
		s1 := state[1][col]
		s2 := state[2][col]
		s3 := state[3][col]

		// Матричное умножени
		state[0][col] = gmul(s0, 2) ^ gmul(s1, 3) ^ s2 ^ s3
		state[1][col] = s0 ^ gmul(s1, 2) ^ gmul(s2, 3) ^ s3
		state[2][col] = s0 ^ s1 ^ gmul(s2, 2) ^ gmul(s3, 3)
		state[3][col] = gmul(s0, 3) ^ s1 ^ s2 ^ gmul(s3, 2)
	}
}

// gmul — умножение в поле GF(2^8)
func gmul(a, b Byte) Byte {
	var p Byte = 0
	for i := 0; i < 8; i++ {
		if b&1 != 0 {
			p ^= a
		}
		hiBitSet := a & 0x80
		a <<= 1
		if hiBitSet != 0 {
			a ^= 0x1b
		}
		b >>= 1
	}
	return p
}
