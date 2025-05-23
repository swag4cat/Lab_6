package aes

// Шифрует блоки в режиме CBC
func AES_CBC_encrypt(blocks []Block, roundKeys []Block, iv Block) []Block {
	var result []Block
	prev := iv // Инициализируем начальным вектором (IV)

	for _, plain := range blocks {
		// XOR текущего блока с предыдущим зашифрованным блоком (или IV для первого блока)
		xored := XorBlocks(plain, prev)

		// Шифруем результат XOR
		encrypted := EncryptBlock(xored, roundKeys)

		result = append(result, encrypted)
		prev = encrypted
	}
	return result
}

// Расшифровывает блоки в режиме CBC
func AES_CBC_decrypt(blocks []Block, roundKeys []Block, iv Block) []Block {
	var result []Block
	prev := iv // Инициализируем начальным вектором (IV)

	for _, cipher := range blocks {
		// Расшифровываем текущий блок
		decrypted := DecryptBlock(cipher, roundKeys)

		// XOR результата с предыдущим зашифрованным блоком (или IV для первого блока)
		plain := XorBlocks(decrypted, prev)

		result = append(result, plain)
		prev = cipher // Запоминаем текущий зашифрованный блок для следующей итерации
	}
	return result
}
