#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

using namespace std;

const double EPS = 1e-3;
const int MAX_ITER = 15;

// Проверка диагонального преобладания
bool checkDiagonalDominance(const vector<vector<double>>& A) {
    int n = A.size();
    for (int i = 0; i < n; ++i) {
        double diag = fabs(A[i][i]);
        double sum = 0;
        for (int j = 0; j < n; ++j) {
            if (i != j) sum += fabs(A[i][j]);
        }
        if (diag <= sum) return false;
    }
    return true;
}

// Проверка положительной определённости (критерий Сильвестра)
bool isPositiveDefinite(const vector<vector<double>>& A) {
    int n = A.size();
    for (int k = 1; k <= n; ++k) {
        double det = 1.0;
        for (int i = 0; i < k; ++i) {
            det *= A[i][i];
        }
        if (det <= 0) return false;
    }
    return true;
}

// Печать таблицы итераций
void printSolutionTable(const vector<vector<double>>& history) {
    int n = history[0].size();
    cout << fixed << setprecision(5);

    // Верхняя граница
    cout << "\n╔";
    for (int i = 0; i < n + 2; ++i)
        cout << "════════════════╦";
    cout << "\b╗\n";

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

// Преобразование к каноническому виду: x = Cx + f
void transformToCanonical(const vector<vector<double>>& A, const vector<double>& b,
                          vector<vector<double>>& C, vector<double>& f) {
    int n = A.size();
    C = A;
    f = b;
    for (int i = 0; i < n; ++i) {
        double diag = A[i][i];
        f[i] = b[i] / diag;
        for (int j = 0; j < n; ++j) {
            if (i == j)
                C[i][j] = 0;
            else
                C[i][j] = -A[i][j] / diag;
        }
    }
}

// Вывод матрицы C и вектора f
void printCanonicalForm(const vector<vector<double>>& C, const vector<double>& f) {
    cout << "\nМатрица C (коэффициенты итераций):\n";
    for (const auto& row : C) {
        for (double val : row)
            cout << setw(12) << fixed << setprecision(6) << val;
        cout << "\n";
    }
    cout << "\nВектор f (свободные члены):\n";
    for (double val : f)
        cout << setw(12) << fixed << setprecision(6) << val << "\n";
}

// Метод Зейделя
vector<double> seidelMethod(const vector<vector<double>>& A, const vector<double>& b,
                            vector<vector<double>>& history, int& iterations_out) {
    int n = A.size();
    vector<double> x(n, 0.0), prev_x(n, 0.0);

    for (int iter = 0; iter < MAX_ITER; ++iter) {
        for (int i = 0; i < n; ++i) {
            double sum = b[i];
            for (int j = 0; j < n; ++j) {
                if (j < i)
                    sum -= A[i][j] * x[j];
                else if (j > i)
                    sum -= A[i][j] * prev_x[j];
            }
            x[i] = sum / A[i][i];
        }

        history.push_back(x);

        double max_delta = 0;
        for (int i = 0; i < n; ++i) {
            max_delta = max(max_delta, fabs(x[i] - prev_x[i]));
        }

        if (max_delta < EPS) {
            iterations_out = iter + 1;
            break;
        }

        prev_x = x;
    }

    return x;
}

int main() {
    vector<vector<double>> A = {
        {0.87, -0.04, 0.21, -18.0},
        {0.25, -1.23, -0.19, -0.09},
        {-0.21, -0.19, 0.80, -0.13},
        {0.15, -1.31, 0.06, 1.08}
    };

    vector<double> b = {-1.24, 1.08, 2.56, 0.87};

    // Проверка условий
    cout << (checkDiagonalDominance(A) ? "Выполняется диагональное преобладание.\n"
                                       : "Предупреждение: диагональное преобладание не выполняется. Метод может не сойтись.\n");
    cout << (isPositiveDefinite(A) ? "Матрица A положительно определённая.\n"
                                   : "Матрица A не является положительно определённой.\n");

    // Преобразование
    vector<vector<double>> C;
    vector<double> f;
    transformToCanonical(A, b, C, f);
    printCanonicalForm(C, f);

    // Итерации
    vector<vector<double>> history;
    int iterations = 0;
    vector<double> solution = seidelMethod(A, b, history, iterations);

    // Вывод
    cout << "\nРешение системы (метод Зейделя):\n";
    for (int i = 0; i < solution.size(); ++i)
        cout << "x" << (i + 1) << " = " << fixed << setprecision(6) << solution[i] << endl;

    cout << "\nТаблица итераций:\n";
    printSolutionTable(history);

    // Расчёт количества операций
    int n = A.size();
    int operations = 2 * n * n * iterations;
    cout << "\nКоличество операций (по формуле 2n^2 * k): " << operations << endl;

    return 0;
}
