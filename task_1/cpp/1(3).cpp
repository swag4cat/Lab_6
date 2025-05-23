#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

using namespace std;

const int M = 20;  // строки
const int N = 40;  // столбцы
const int GENERATIONS = 100;  // количество поколений

// Вывод матрицы
void printGrid(const vector<vector<int>>& grid) {
    system(CLEAR);
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << (grid[i][j] ? "█" : " ");
        }
        cout << endl;
    }
}

// Подсчёт живых соседей (с учётом замкнутой плоскости)
int countLiveNeighbors(const vector<vector<int>>& grid, int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; ++dx)
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int nx = (x + dx + M) % M;
            int ny = (y + dy + N) % N;
            count += grid[nx][ny];
        }
        return count;
}

// Обновление матрицы по правилам Конвея
vector<vector<int>> nextGeneration(const vector<vector<int>>& grid) {
    vector<vector<int>> newGrid = grid;
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j) {
            int neighbors = countLiveNeighbors(grid, i, j);
            if (grid[i][j]) {
                newGrid[i][j] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            } else {
                newGrid[i][j] = (neighbors == 3) ? 1 : 0;
            }
        }
        return newGrid;
}

// Инициализация Glider'а
void placeGlider(vector<vector<int>>& grid, int x, int y) {
    grid[x][y + 1] = 1;
    grid[x + 1][y + 2] = 1;
    grid[x + 2][y] = 1;
    grid[x + 2][y + 1] = 1;
    grid[x + 2][y + 2] = 1;
}

// Основная функция
int main() {
    vector<vector<int>> grid(M, vector<int>(N, 0));

    srand(time(nullptr)); // Инициализация генератора случайных чисел

    const int NUM_GLIDERS = 5; // Количество глайдеров

    for (int i = 0; i < NUM_GLIDERS; ++i) {
        // Глайдер занимает 3x3 область, поэтому ограничим координаты
        int x = rand() % (M - 3);
        int y = rand() % (N - 3);
        placeGlider(grid, x, y);
    }

    for (int gen = 0; gen < GENERATIONS; ++gen) {
        printGrid(grid);
        grid = nextGeneration(grid);
        this_thread::sleep_for(chrono::milliseconds(200));
    }

    return 0;
}
