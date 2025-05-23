#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    int M, N;
    cout << "Введите количество строк (M): ";
    cin >> M;
    cout << "Введите количество столбцов (N): ";
    cin >> N;

    srand(static_cast<unsigned>(time(0)));

    // Создание и инициализация матрицы
    vector<vector<int>> matrix(M, vector<int>(N));
    unordered_map<int, int> frequency;

    cout << "\nСгенерированная матрица:\n";
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = rand() % 51 + 100; // [100, 150]
            frequency[matrix[i][j]]++;
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }

    // Поиск наиболее часто встречающегося числа
    int mostFrequent = -1;
    int maxCount = 1; // если все числа уникальны, выводим сообщение
    for (const auto& pair : frequency) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            mostFrequent = pair.first;
        }
    }

    if (mostFrequent == -1) {
        cout << "\nВсе числа в матрице уникальны." << endl;
    } else {
        cout << "\nЧисло, которое встречается чаще всего: " << mostFrequent
        << " (встречается " << maxCount << " раз)" << endl;
    }

    return 0;
}
