package main

import (
	"aes-cbc-go/aes"
	"bufio"
	"fmt"
	"os"
)

func main() {
	var inputText string
	var mode int

	// Выбор режима ввода
	fmt.Println("Выберите режим ввода:")
	fmt.Println("1 - Ввести текст вручную")
	fmt.Println("2 - Прочитать текст из файла input.txt")
	fmt.Print("Ваш выбор: ")
	fmt.Scanln(&mode)

	// Обработка ввода
	switch mode {
		case 1:
			fmt.Println("Введите текст для шифрования:")
			scanner := bufio.NewScanner(os.Stdin)
			if scanner.Scan() {
				inputText = scanner.Text()
			}
		case 2:
			content, err := os.ReadFile("input.txt")
			if err != nil {
				fmt.Println("Ошибка чтения input.txt:", err)
				return
			}
			inputText = string(content)
			fmt.Printf("Прочитано %d байт из input.txt\n", len(content))
		default:
			fmt.Println("Некорректный выбор режима!")
			return
	}

	// Генерация ключа и IV
	key := aes.GenerateRandomKey(16)
	iv := aes.GenerateRandomIV()

	// Сохранение и вывод ключа
	aes.PrintKey(key)
	aes.SaveKeyToFile("aes_key.bin", key)
	println("Ключ сохранен в файл: aes_key.bin")

	// Вывод IV
	aes.PrintBlock(iv, "\nIV:")

	// Разбивка текста на блоки
	plaintextBlocks := aes.TextToBlocks(inputText)

	// Вывод исходных блоков
	fmt.Println("\nБлоки исходного текста:")
	for i, blk := range plaintextBlocks {
		fmt.Printf("Блок %d:\n", i+1)
		aes.PrintBlock(blk, "")
	}

	// Генерация раундовых ключей
	roundKeys := aes.ExpandKey(key)

	// Шифрование
	encryptedBlocks := aes.AES_CBC_encrypt(plaintextBlocks, roundKeys, iv)

	// Сохранение зашифрованных данных
	cipherFile, _ := os.Create("cipher.bin")
	for _, blk := range encryptedBlocks {
		cipherFile.Write(aes.BlockToBytes(blk))
	}
	cipherFile.Close()
	fmt.Println("\nЗашифрованные данные записаны в cipher.bin")

	// Вывод зашифрованных блоков
	fmt.Println("\nЗашифрованные блоки:")
	for i, blk := range encryptedBlocks {
		fmt.Printf("Блок %d:\n", i+1)
		aes.PrintBlock(blk, "")
	}

	// Расшифровка
	decryptedBlocks := aes.AES_CBC_decrypt(encryptedBlocks, roundKeys, iv)
	decryptedText := aes.BlocksToText(decryptedBlocks)

	// Сохранение результата
	os.WriteFile("output.txt", []byte(decryptedText), 0644)
	fmt.Println("Расшифрованный текст записан в output.txt")

	// Вывод результата
	fmt.Println("\nРасшифрованный текст:")
	fmt.Println(decryptedText)
}
