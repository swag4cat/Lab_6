#include <iostream>
#include <fstream>
#include <string>
#include <limits>

#include "aes.h"

int main() {
    // =============================================
    //              1. Ввод данных
    // =============================================
    std::string inputText;
    int mode = 0;

    // Выбор режима ввода
    std::cout << "Выберите режим ввода:\n";
    std::cout << "1 - Ввести текст вручную\n";
    std::cout << "2 - Прочитать текст из файла input.txt\n";
    std::cout << "Ваш выбор: ";
    std::cin >> mode;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Обработка выбора режима
    if (mode == 1) {
        std::cout << "Введите текст для шифрования: \n";
        std::getline(std::cin, inputText);
    }
    else if (mode == 2) {
        std::ifstream in("input.txt");
        if (!in) {
            std::cerr << "Файл input.txt не найден!" << std::endl;
            return 1;
        }
        // Чтение всего содержимого файла
        inputText.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
        in.close();
        std::cout << "Прочитано " << inputText.size() << " байт из input.txt\n";
    }
    else {
        std::cerr << "Некорректный выбор режима!\n";
        return 1;
    }

    // ===============================================
    // 2. Генерация ключа и вектора инициализации (IV)
    // ===============================================
    std::vector<Byte> masterKey;
    Block iv;
    generateRandomKey(masterKey); // Генерация случайного ключа
    generateRandomIV(iv); // Генерация случайного IV

    // Вывод сгенерированного ключа
    printKey(masterKey);
    saveKeyToFile("aes_key.bin", masterKey);
    std::cout << "Ключ сохранен в файл: aes_key.bin\n";

    // Вывод IV
    printBlock(iv, "\nIV:");

    // =============================================
    //   3. Подготовка данных (разбиение на блоки)
    // =============================================
    std::vector<Block> plaintextBlocks = textToBlocks(inputText);

    // Вывод исходных блоков
    std::cout << "\nБлоки исходного текста:\n";
    for (size_t i = 0; i < plaintextBlocks.size(); ++i) {
        std::cout << "Блок " << i + 1 << ":\n";
        printBlock(plaintextBlocks[i]);
    }

    // =============================================
    //               4. Шифрование
    // =============================================
    // Расширение ключа для всех раундов
    std::vector<Block> roundKeys = expandKey(masterKey);

    // Шифрование в режиме CBC
    std::vector<Block> ciphertextBlocks = AES_CBC_encrypt(plaintextBlocks, roundKeys, iv);

    // Сохранение зашифрованных данных в бинарный файл
    std::ofstream cipherOut("cipher.bin", std::ios::binary);
    if (!cipherOut) {
        std::cerr << "Не удалось создать файл cipher.bin\n";
    }
    else {
        for (const Block& block : ciphertextBlocks) {
            for (int col = 0; col < 4; ++col) {
                for (int row = 0; row < 4; ++row) {
                    cipherOut.put(static_cast<char>(block[row][col]));
                }
            }
        }
        cipherOut.close();
        std::cout << "\nЗашифрованные данные записаны в cipher.bin\n";
    }

    // Вывод зашифрованных блоков
    std::cout << "\nЗашифрованные блоки:\n";
    for (size_t i = 0; i < ciphertextBlocks.size(); ++i) {
        std::cout << "Блок " << i + 1 << ":\n";
        printBlock(ciphertextBlocks[i]);
    }

    // =============================================
    //               5. Дешифрование
    // =============================================
    std::vector<Block> decryptedBlocks = AES_CBC_decrypt(ciphertextBlocks, roundKeys, iv);
    std::string decryptedText = blocksToText(decryptedBlocks);

    // Сохранение расшифрованного текста
    std::ofstream out("output.txt");
    if (!out) {
        std::cerr << "Не удалось создать файл output.txt\n";
    }
    else {
        out << decryptedText;
        out.close();
        std::cout << "Расшифрованный текст записан в output.txt\n";
    }

    // Очистка пробелов в конце
    decryptedText.erase(decryptedText.find_last_not_of(' ') + 1);

    // Вывод результата
    std::cout << "\nРасшифрованный текст:\n" << decryptedText << "\n";

    return 0;
}
