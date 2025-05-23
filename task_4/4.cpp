#include <iostream>
#include <utility>
#include <algorithm>

using namespace std;

// Функция для подсчёта количества единиц в двоичном представлении числа
int count_ones(unsigned long long num) {
    int count = 0;
    while (num > 0) {
        count += num & 1;
        num >>= 1;
    }
    return count;
}

int main() {
    unsigned long long N;
    cout << "Ввод: ";
    cin >> N;

    pair<unsigned long long, unsigned long long> best_pair = {0, N};
    int max_bananas = count_ones(0) + count_ones(N);

    for (unsigned long long a = 1; a <= N / 2; ++a) {
        unsigned long long b = N - a;
        int current_bananas = count_ones(a) + count_ones(b);

        if (current_bananas > max_bananas) {
            max_bananas = current_bananas;
            best_pair = {a, b};
        } else if (current_bananas == max_bananas) {
            // Если количество бананов одинаковое, выбираем пару с большей разностью
            if ((b - a) > (best_pair.second - best_pair.first)) {
                best_pair = {a, b};
            }
        }
    }

    cout << "Вывод: " << best_pair.first << " " << best_pair.second << endl;

    return 0;
}
