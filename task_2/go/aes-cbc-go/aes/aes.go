package aes

import (
	"bytes"
	"crypto/aes"
	"crypto/cipher"
	"fmt"
)

// PKCS7Pad добавляет паддинг PKCS7 к данным для выравнивания по размеру блока
func PKCS7Pad(data []byte, blockSize int) []byte {
	padLen := blockSize - (len(data) % blockSize)
	padding := bytes.Repeat([]byte{byte(padLen)}, padLen)
	return append(data, padding...)
}

// PKCS7Unpad удаляет паддинг PKCS7 и проверяет его корректность
func PKCS7Unpad(data []byte) ([]byte, error) {
	length := len(data)
	if length == 0 {
		return nil, fmt.Errorf("пустой ввод")
	}
	padLen := int(data[length-1])
	if padLen > length || padLen == 0 {
		return nil, fmt.Errorf("неверный паддинг")
	}
	// Проверяем, что все байты паддинга корректны
	for _, b := range data[length-padLen:] {
		if int(b) != padLen {
			return nil, fmt.Errorf("неверный паддинг")
		}
	}
	return data[:length-padLen], nil
}

// EncryptAES_CBC шифрует данные алгоритмом AES в режиме CBC
func EncryptAES_CBC(plainText []byte, key []Byte, iv Block) ([]byte, error) {
	ivBytes := BlockToBytes(iv)
	block, err := aes.NewCipher(key) // Создаем новый cipher.Block
	if err != nil {
		return nil, err
	}
	plainText = PKCS7Pad(plainText, block.BlockSize()) // Добавляем паддинг
	cipherText := make([]byte, len(plainText))
	mode := cipher.NewCBCEncrypter(block, ivBytes) // Создаем CBC encryptor
	mode.CryptBlocks(cipherText, plainText) // Шифруем данные
	return cipherText, nil
}

// DecryptAES_CBC расшифровывает данные алгоритмом AES в режиме CBC
func DecryptAES_CBC(cipherText []byte, key []Byte, iv Block) ([]byte, error) {
	ivBytes := BlockToBytes(iv)
	block, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}
	// Проверяем, что размер шифротекста кратен размеру блока
	if len(cipherText)%block.BlockSize() != 0 {
		return nil, fmt.Errorf("длина шифротекста должна быть кратна размеру блока")
	}
	plainText := make([]byte, len(cipherText))
	mode := cipher.NewCBCDecrypter(block, ivBytes) // Создаем CBC decryptor
	mode.CryptBlocks(plainText, cipherText) // Расшифровываем данные
	return PKCS7Unpad(plainText) // Удаляем паддинг
}
