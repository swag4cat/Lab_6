#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <stdexcept>
#include <string>

using namespace std;

// Печать расширенной матрицы
void printMatrix(const vector<vector<double>>& matrix, const vector<double>& b) {
    int n = matrix.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << setw(10) << matrix[i][j] << " ";
        }
        cout << "| " << setw(10) << b[i] << endl;
    }
    cout << endl;
}

// Табличный вывод результата
void printSolutionTable(const vector<vector<double>>& history) {
    int n = history[0].size();
    cout << fixed << setprecision(5);

    // Верхняя граница
    cout << "\n╔";
    for (int i = 0; i < n + 2; ++i)
        cout << "════════════════╦";
    cout << "\b╗\n"; // Удалить последнюю лишнюю черту и поставить угол

    // Заголовок
    cout << "║" << setw(16) << left << "       N";
    for (int i = 0; i < n; ++i) {
        cout << "║" << setw(16) << left << ("       x" + to_string(i + 1));
    }
    cout << "║" << setw(16) << left << "       εn" << " ║\n";

    // Разделитель
    cout << "╠";
    for (int i = 0; i < n + 2; ++i)
        cout << "════════════════╬";
    cout << "\b╣\n";

    // Данные
    for (size_t step = 0; step < history.size(); ++step) {
        cout << "║" << setw(16) << left << ("       " + to_string(step + 1));
        double max_eps = 0;
        for (int i = 0; i < n; ++i) {
            cout << "║   " << setw(13) << left << history[step][i];
            if (step > 0) {
                max_eps = max(max_eps, fabs(history[step][i] - history[step - 1][i]));
            }
        }
        if (step == 0)
            cout << "║" << setw(16) << left << "       -";
        else
            cout << "║" << setw(16) << left << max_eps;
        cout << "║\n";
    }

    // Нижняя граница
    cout << "╚";
    for (int i = 0; i < n + 2; ++i)
        cout << "════════════════╩";
    cout << "\b╝\n";
}


// Метод Гаусса с выбором главного элемента
vector<double> gaussWithPivoting(vector<vector<double>> A, vector<double> b, vector<vector<double>>& steps_out) {
    int n = A.size();
    vector<int> perm(n);
    for (int i = 0; i < n; ++i) perm[i] = i;

    // Прямой ход
    for (int k = 0; k < n; ++k) {
        int max_row = k;
        double max_val = abs(A[k][k]);
        for (int i = k + 1; i < n; ++i) {
            if (abs(A[i][k]) > max_val) {
                max_val = abs(A[i][k]);
                max_row = i;
            }
        }

        if (max_row != k) {
            swap(A[k], A[max_row]);
            swap(b[k], b[max_row]);
            swap(perm[k], perm[max_row]);
        }

        if (abs(A[k][k]) < 1e-10) {
            throw runtime_error("Матрица вырождена или плохо обусловлена");
        }

        double div = A[k][k];
        for (int j = k; j < n; ++j) {
            A[k][j] /= div;
        }
        b[k] /= div;

        for (int i = k + 1; i < n; ++i) {
            double factor = A[i][k];
            for (int j = k; j < n; ++j) {
                A[i][j] -= factor * A[k][j];
            }
            b[i] -= factor * b[k];
        }

        cout << "Шаг " << k + 1 << " (после исключения x" << k + 1 << "):" << endl;
        printMatrix(A, b);
    }

    // Обратный ход
    vector<double> x(n);
    for (int i = n - 1; i >= 0; --i) {
        x[i] = b[i];
        for (int j = i + 1; j < n; ++j) {
            x[i] -= A[i][j] * x[j];
        }
    }

    // Восстановление порядка
    vector<double> x_sorted(n);
    for (int i = 0; i < n; ++i) {
        x_sorted[perm[i]] = x[i];
    }

    // Добавим решение в таблицу итераций
    steps_out.push_back(x_sorted);
    return x_sorted;
}

int main() {
    vector<vector<double>> A = {
        {0.87, -0.04, 0.21, -18.0},
        {0.25, -1.23, -0.19, -0.09},
        {-0.21, -0.19, 0.80, -0.13},
        {0.15, -1.31, 0.06, 1.08}
    };

    vector<double> b = {-1.24, 1.08, 2.56, 0.87};
    vector<vector<double>> history; // Для хранения шагов

    cout << "Исходная система:\n";
    printMatrix(A, b);

    try {
        vector<double> solution = gaussWithPivoting(A, b, history);

        cout << "\nРешение системы (вектор X):\n";
        cout << fixed << setprecision(6);
        for (int i = 0; i < solution.size(); ++i) {
            cout << "x" << i + 1 << " = " << solution[i] << endl;
        }

        cout << "\nТаблица результата:\n";
        printSolutionTable(history);

    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }

    return 0;
}
