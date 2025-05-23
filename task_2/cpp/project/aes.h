#pragma once
#include <vector>
#include <array>
#include <string>

// Типы данных
using Byte = unsigned char; // Байт (8 бит)
using Block = std::array<std::array<Byte, 4>, 4>; // Блок AES (4x4 байта)

// =============================================
//          Генерация случайных данных
// =============================================

// Генерирует случайный ключ заданной длины (по умолчанию 16 байт)
void generateRandomKey(std::vector<Byte>& key, size_t length = 16);

// Генерирует случайный вектор инициализации (IV)
void generateRandomIV(Block& iv);

// =============================================
//              Ввод-вывод и отладка
// =============================================

// Выводит ключ в консоль
void printKey(const std::vector<Byte>& key);

// Выводит блок в консоль с опциональным заголовком
void printBlock(const Block& block, const std::string& title = "");

// Сохраняет бинарные данные в файл
void saveToFile(const std::string& filename, const std::vector<Byte>& data);

// Сохраняет блок в файл
void saveBlockToFile(const std::string& filename, const Block& block);

// Сохраняет ключ в файл
void saveKeyToFile(const std::string& filename, const std::vector<Byte>& key);

// =============================================
//             Преобразование данных
// =============================================

// Конвертирует строку в блоки AES
std::vector<Block> textToBlocks(const std::string& text);

// Конвертирует блоки AES обратно в строку и удаляет PKCS#7 паддинг
std::string blocksToText(const std::vector<Block>& blocks);

// Расширяет ключ для всех раундов шифрования
std::vector<Block> expandKey(const std::vector<Byte>& key);

// XOR двух блоков
Block xorBlocks(const Block& a, const Block& b);

// =============================================
//      Базовые операции AES (шифрование)
// =============================================

// Шифрует один блок с использованием ключей раундов
Block encryptBlock(const Block& input, const std::vector<Block>& roundKeys);

// Замена байт по S-Box (SubBytes)
void subBytes(Block& state);

// Циклический сдвиг строк (ShiftRows)
void shiftRows(Block& state);

// Смешивание столбцов (MixColumns)
void mixColumns(Block& state);

// Умножение в поле Галуа (GF(2^8))
Byte gmul(Byte a, Byte b);

// =============================================
//      Базовые операции AES (дешифрование)
// =============================================

// Дешифрует один блок с использованием ключей раундов
Block decryptBlock(const Block& input, const std::vector<Block>& roundKeys);

// Обратная замена байт (InvSubBytes)
void invSubBytes(Block& state);

// Обратный сдвиг строк (InvShiftRows)
void invShiftRows(Block& state);

// Обратное смешивание столбцов (InvMixColumns)
void invMixColumns(Block& state);

// =============================================
//      Режим CBC (Cipher Block Chaining)
// =============================================

// Шифрование в режиме CBC
std::vector<Block> AES_CBC_encrypt(
    const std::vector<Block>& plaintextBlocks,
    const std::vector<Block>& roundKeys,
    const Block& iv
);

// Дешифрование в режиме CBC
std::vector<Block> AES_CBC_decrypt(
    const std::vector<Block>& ciphertextBlocks,
    const std::vector<Block>& roundKeys,
    const Block& iv
);
