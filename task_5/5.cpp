#include <iostream>
#include <vector>
#include <climits>
#include <random>
#include <algorithm>

using namespace std;

// Функция для генерации случайной доски
vector<vector<bool>> generateRandomBoard(int n, int m) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, 1);

    vector<vector<bool>> board(n, vector<bool>(m));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            board[i][j] = distrib(gen);
        }
    }
    return board;
}

// Функция для печати доски
void printBoard(const vector<vector<bool>>& board) {
    for (const auto& row : board) {
        for (bool cell : row) {
            cout << (cell ? "H " : "T "); // H - орел (Head), T - решка (Tail)
        }
        cout << endl;
    }
}

// Функция для проверки, является ли доска шахматной с заданным первым значением
bool isChessPattern(const vector<vector<bool>>& board, bool firstValue) {
    bool expected = firstValue;
    for (const auto& row : board) {
        bool currentRowExpected = expected;
        for (bool cell : row) {
            if (cell != currentRowExpected) {
                return false;
            }
            currentRowExpected = !currentRowExpected;
        }
        expected = !expected;
    }
    return true;
}

// Функция для переворота двух соседних монет с выводом шага
void flipCoins(vector<vector<bool>>& board, int i1, int j1, int i2, int j2, int stepNumber) {
    cout << "Шаг " << stepNumber << ": переворот (" << i1 << ", " << j1 << ") и (" << i2 << ", " << j2 << ")\n";
    board[i1][j1] = !board[i1][j1];
    board[i2][j2] = !board[i2][j2];
    printBoard(board);
    cout << "----------------------------------\n";
}

// Функция для поиска минимального количества ходов с выводом шагов
int findMinMoves(vector<vector<bool>> board, bool firstValue) {
    int moves = 0;
    int step = 1;
    int n = board.size();
    int m = board[0].size();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            bool expected = (i + j) % 2 == 0 ? firstValue : !firstValue;
            if (board[i][j] != expected) {
                if (j + 1 < m) {
                    flipCoins(board, i, j, i, j + 1, step++);
                    moves++;
                } else if (i + 1 < n) {
                    flipCoins(board, i, j, i + 1, j, step++);
                    moves++;
                } else {
                    return INT_MAX;
                }
            }
        }
    }

    // Проверим, получим ли мы нужный узор
    if (isChessPattern(board, firstValue)) {
        return moves;
    } else {
        return INT_MAX;
    }
}


int main() {
    int n, m;
    cout << "Введите размеры доски N и M: ";
    cin >> n >> m;

    // Генерация случайной доски
    vector<vector<bool>> board = generateRandomBoard(n, m);

    cout << "Исходная доска:" << endl;
    printBoard(board);
    cout << "----------------------------------\n";

    // Проверяем оба возможных шахматных узора
    int moves1 = findMinMoves(board, true);  // Первый вариант: начинается с H
    int moves2 = findMinMoves(board, false); // Второй вариант: начинается с T

    if (moves1 == INT_MAX && moves2 == INT_MAX) {
        cout << "Невозможно привести доску к шахматному узору." << endl;
    } else {
        int minMoves = min(moves1, moves2);
        cout << "Минимальное количество ходов: " << minMoves << endl;

        // Выводим, какой узор получится
        if (moves1 < moves2) {
            cout << "Получится узор, начинающийся с H в левом верхнем углу" << endl;
        } else {
            cout << "Получится узор, начинающийся с T в левом верхнем углу" << endl;
        }
    }

    return 0;
}
