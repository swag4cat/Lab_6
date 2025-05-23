#include <iostream>
#include <vector>
#include <cstdlib>   // для rand() и srand()
#include <ctime>     // для time()

using namespace std;

int main() {
    int N;
    cout << "Введите порядок квадратной матрицы N: ";
    cin >> N;

    // Инициализация генератора случайных чисел
    srand(static_cast<unsigned>(time(0)));

    // Создание и инициализация матрицы
    vector<vector<int>> matrix(N, vector<int>(N));
    cout << "\nСгенерированная матрица:\n";
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = rand() % 201 - 100; // от -100 до 100
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }

    // Поиск строки с наибольшим числом положительных элементов
    int maxPositives = 0;
    int maxRowIndex = 0;

    for (int i = 0; i < N; ++i) {
        int positiveCount = 0;
        for (int j = 0; j < N; ++j) {
            if (matrix[i][j] > 0) {
                ++positiveCount;
            }
        }
        if (positiveCount > maxPositives) {
            maxPositives = positiveCount;
            maxRowIndex = i;
        }
    }

    // Инициализация нового массива найденной строкой
    vector<int> resultRow = matrix[maxRowIndex];

    // Вывод результата
    cout << "\nСтрока с наибольшим количеством положительных чисел (номер " << maxRowIndex << "):\n";
    for (int val : resultRow) {
        cout << val << " ";
    }
    cout << endl;

    return 0;
}
