#include <iostream>
#include <iomanip>
#include <random>
#include <fstream>

#include "aes.h"

// =============================================
//          Генерация случайных данных
// =============================================

// Генерирует случайный ключ заданной длины (по умолчанию 16 байт)
void generateRandomKey(std::vector<Byte>& key, size_t length) {
    std::random_device rd;             // Источник энтропии
    std::mt19937 gen(rd());            // Генератор случайных чисел
    std::uniform_int_distribution<> dis(0, 255); // Диапазон байтов [0, 255]

    key.resize(length);
    for (size_t i = 0; i < length; ++i) {
        key[i] = static_cast<Byte>(dis(gen)); // Заполнение ключа случайными байтами
    }
}

// Генерирует случайный вектор инициализации (IV)
void generateRandomIV(Block& iv) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col)
            iv[row][col] = static_cast<Byte>(dis(gen)); // Заполнение IV случайными байтами
}

// =============================================
//              Ввод-вывод и отладка
// =============================================

// Выводит ключ в консоль
void printKey(const std::vector<Byte>& key) {
    std::cout << "Ключ (16 байт): ";
    for (Byte b : key) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<int>(b) << " "; // Форматированный вывод байта
    }
    std::cout << std::dec << "\n"; // Возврат к десятичному формату
}

// Выводит блок в консоль с заголовком
void printBlock(const Block& block, const std::string& title) {
    if (!title.empty()) std::cout << title << std::endl;
    for (const auto& row : block) {
        for (Byte b : row) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(b) << " "; // Вывод байта
        }
        std::cout << std::endl;
    }
    std::cout << std::dec; // Сброс формата вывода
}

// Сохраняет ключ в бинарный файл
void saveKeyToFile(const std::string& filename, const std::vector<Byte>& key) {
    std::ofstream file(filename, std::ios::binary); // Открытие файла в бинарном режиме
    if (!file) {
        std::cerr << "Ошибка при создании файла для ключа: " << filename << "\n";
        return;
    }
    file.write(reinterpret_cast<const char*>(key.data()), key.size()); // Запись байтов ключа в файл
}

// =============================================
//             Преобразование данных
// =============================================

// Конвертирует строку в блоки AES
std::vector<Block> textToBlocks(const std::string& text) {
    std::string padded = text;

    // Заполнение пробелами для выравнивания по 16 байт
    while (padded.size() % 16 != 0) {
        padded += ' ';
    }

    std::vector<Block> blocks;
    for (size_t i = 0; i < padded.size(); i += 16) {
        Block block{};
        // Заполнение блока по столбцам (формат AES: столбцово-ориентированная матрица)
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                block[row][col] = static_cast<Byte>(padded[i + col * 4 + row]);
            }
        }
        blocks.push_back(block);
    }

    return blocks;
}

// Конвертирует блоки AES обратно в строку и удаляет PKCS#7 паддинг
std::string blocksToText(const std::vector<Block>& blocks) {
    std::string text;
    // Сборка строки из блоков по столбцам
    for (const auto& block : blocks) {
        for (int col = 0; col < 4; ++col)
            for (int row = 0; row < 4; ++row)
                text += static_cast<char>(block[row][col]);
    }

    // Удаление паддинга PKCS#7
    if (!text.empty()) {
        unsigned char pad = static_cast<unsigned char>(text.back());
        if (pad > 0 && pad <= 16 && text.size() >= pad) {
            bool valid = true;
            for (int i = 0; i < pad; ++i) {
                if (static_cast<unsigned char>(text[text.size() - 1 - i]) != pad) {
                    valid = false;
                    break;
                }
            }
            if (valid) text.erase(text.size() - pad);
        }
    }

    return text;
}

// Расширяет ключ для всех раундов шифрования
std::vector<Block> expandKey(const std::vector<Byte>& key) {
    static const Byte sbox[256] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
    };

    static const Byte Rcon[11] = {
        0x00, 0x01, 0x02, 0x04, 0x08,
        0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
    };

    std::vector<Byte> expandedKey(176); // Общая длина ключа: 11 * 16 байт
    for (int i = 0; i < 16; ++i)
        expandedKey[i] = key[i]; // Копируем начальный (основной) ключ

    int bytesGenerated = 16;
    int rconIndex = 1;
    Byte temp[4];

    while (bytesGenerated < 176) {

        // Последние 4 байта предыдущего блока
        for (int i = 0; i < 4; ++i)
            temp[i] = expandedKey[bytesGenerated - 4 + i];

        // Каждые 16 байт — выполнение ключевой трансформации
        if (bytesGenerated % 16 == 0) {

            // RotWord — циклический сдвиг влево
            Byte t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;

            // SubWord — замена байтов через S-box
            for (int i = 0; i < 4; ++i)
                temp[i] = sbox[temp[i]];

            // XOR с Rcon
            temp[0] ^= Rcon[rconIndex++];
        }

        // Генерация следующих 4 байт ключа
        for (int i = 0; i < 4; ++i) {
            expandedKey[bytesGenerated] = expandedKey[bytesGenerated - 16] ^ temp[i];
            ++bytesGenerated;
        }
    }

    // Разбиение расширенного ключа на 11 блоков по 16 байт (раундовые ключи)
    std::vector<Block> roundKeys;
    for (int i = 0; i < 11; ++i) {
        Block block{};
        for (int j = 0; j < 16; ++j) {
            block[j % 4][j / 4] = expandedKey[i * 16 + j]; // Построение блока по столбцам
        }
        roundKeys.push_back(block);
    }

    return roundKeys;
}

// XOR двух блоков
Block xorBlocks(const Block& a, const Block& b) {
    Block result{};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            result[i][j] = a[i][j] ^ b[i][j];
    return result;
}

// =============================================
//      Базовые операции AES (шифрование)
// =============================================

// Шифрует один блок с использованием ключей раундов
Block encryptBlock(const Block& input, const std::vector<Block>& roundKeys) {
    Block state = xorBlocks(input, roundKeys[0]); // AddRoundKey (начальная перестановка)

    // Основные 9 раундов AES
    for (int round = 1; round < 10; ++round) {
        subBytes(state);        // Заменяем байты через S-box
        shiftRows(state);       // Сдвигаем строки
        mixColumns(state);      // Смешиваем столбцы
        state = xorBlocks(state, roundKeys[round]); // Добавление раундового ключа
    }

    // Финальный (10-й) раунд без MixColumns
    subBytes(state);
    shiftRows(state);
    state = xorBlocks(state, roundKeys[10]);

    return state;
}

// Замена байт по S-Box (SubBytes)
void subBytes(Block& state) {
    static const Byte sbox[256] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
    };

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            state[i][j] = sbox[state[i][j]];
}

// Циклический сдвиг строк (ShiftRows)
void shiftRows(Block& state) {
    Block temp = state;

    for (int i = 1; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            state[i][j] = temp[i][(j + i) % 4];
}

// Смешивание столбцов (MixColumns)
void mixColumns(Block& state) {
    for (int col = 0; col < 4; ++col) {
        Byte s0 = state[0][col];
        Byte s1 = state[1][col];
        Byte s2 = state[2][col];
        Byte s3 = state[3][col];

        state[0][col] = gmul(s0, 2) ^ gmul(s1, 3) ^ s2 ^ s3;
        state[1][col] = s0 ^ gmul(s1, 2) ^ gmul(s2, 3) ^ s3;
        state[2][col] = s0 ^ s1 ^ gmul(s2, 2) ^ gmul(s3, 3);
        state[3][col] = gmul(s0, 3) ^ s1 ^ s2 ^ gmul(s3, 2);
    }
}

// Умножение в поле Галуа (GF(2^8))
Byte gmul(Byte a, Byte b) {
    Byte p = 0;
    for (int i = 0; i < 8; ++i) {
        if (b & 1) p ^= a;
        bool hi = a & 0x80;
        a <<= 1;
        if (hi) a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

// =============================================
//      Базовые операции AES (дешифрование)
// =============================================

// Дешифрует один блок с использованием ключей раундов
Block decryptBlock(const Block& input, const std::vector<Block>& roundKeys) {

    // Начальное добавление ключа раунда (последнего)
    Block state = xorBlocks(input, roundKeys[10]);

    // Основные раунды
    for (int round = 9; round >= 1; --round) {
        invShiftRows(state);
        invSubBytes(state);
        state = xorBlocks(state, roundKeys[round]);
        invMixColumns(state);
    }

     // Последний раунд
    invShiftRows(state);
    invSubBytes(state);
    state = xorBlocks(state, roundKeys[0]);

    return state;
}

// Обратная замена байт (InvSubBytes)
void invSubBytes(Block& state) {
    static const Byte invSBox[256] = {
        0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
        0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
        0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
        0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
        0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
        0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
        0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
        0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
        0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
        0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
        0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
        0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
        0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
        0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
        0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
        0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
    };

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            state[i][j] = invSBox[state[i][j]];
}

// Обратный сдвиг строк (InvShiftRows)
void invShiftRows(Block& state) {
    Block temp = state;

    for (int i = 1; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            state[i][j] = temp[i][(j - i + 4) % 4];
}

// Обратное смешивание столбцов (InvMixColumns)
void invMixColumns(Block& state) {
    for (int col = 0; col < 4; ++col) {
        Byte s0 = state[0][col];
        Byte s1 = state[1][col];
        Byte s2 = state[2][col];
        Byte s3 = state[3][col];

        state[0][col] = gmul(s0, 0x0e) ^ gmul(s1, 0x0b) ^ gmul(s2, 0x0d) ^ gmul(s3, 0x09);
        state[1][col] = gmul(s0, 0x09) ^ gmul(s1, 0x0e) ^ gmul(s2, 0x0b) ^ gmul(s3, 0x0d);
        state[2][col] = gmul(s0, 0x0d) ^ gmul(s1, 0x09) ^ gmul(s2, 0x0e) ^ gmul(s3, 0x0b);
        state[3][col] = gmul(s0, 0x0b) ^ gmul(s1, 0x0d) ^ gmul(s2, 0x09) ^ gmul(s3, 0x0e);
    }
}

// =============================================
//      Режим CBC (Cipher Block Chaining)
// =============================================

// Шифрование в режиме CBC
std::vector<Block> AES_CBC_encrypt(const std::vector<Block>& plaintextBlocks,
                                   const std::vector<Block>& roundKeys,
                                   const Block& iv) {
    std::vector<Block> ciphertextBlocks;
    Block previous = iv;

    for (const Block& plain : plaintextBlocks) {
        Block xored = xorBlocks(plain, previous); // CBC: XOR с IV или предыдущим шифрблоком
        Block encrypted = encryptBlock(xored, roundKeys); // Шифруем блок
        ciphertextBlocks.push_back(encrypted);
        previous = encrypted; // Обновляем для следующего блока
    }

    return ciphertextBlocks;
                                   }

// Дешифрование в режиме CBC
std::vector<Block> AES_CBC_decrypt(const std::vector<Block>& ciphertextBlocks,
                                   const std::vector<Block>& roundKeys,
                                   const Block& iv) {
    std::vector<Block> decryptedBlocks;
    Block previous = iv;

    for (const Block& cipher : ciphertextBlocks) {
        Block decrypted = decryptBlock(cipher, roundKeys); // Обычная AES-дешифровка
        Block plain = xorBlocks(decrypted, previous); // XOR с предыдущим зашифрованным
        decryptedBlocks.push_back(plain);
        previous = cipher; // Текущий становится предыдущим
    }

    return decryptedBlocks;
                                   }
