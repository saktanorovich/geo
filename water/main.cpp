#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

const double eps = 1e-6;
const double oo  = 1e+6;
const double pi  = acos(-1);
const int max_t  = 40;

inline void init() {
    ios::sync_with_stdio(false);
    cout.precision(3);
    cout.setf(ios::fixed);
}

template<class T>
T read() {
    T value;
    cin >> value;
    return value;
}

int sign(double x) {
    if (x + eps < 0) return -1;
    if (x - eps > 0) return +1;
    return 0;
}

struct tank {
public:
    int c;
    int r;
public:
    double volume(double l) const {
        if (sign(l - (c - r)) < 0) {
            return 0;
        }
        if (sign(l - (c + r)) > 0) {
            return 4 * pi * r * r * r / 3;
        }
        double h = l - c + r;
        return pi * h * h * (3 * r - h) / 3;
    }
    double height(double l) const {
        if (sign(l - (c - r)) < 0) {
            return 0;
        }
        if (sign(l - (c + r)) > 0) {
            return 2 * r;
        }
        double h = l - c + r;
        return h;
    }
};
typedef vector<tank> tanks;

double fill(const tanks& tanks, int V) {
    double lo = 0.0, hi = +oo;
    for (int it = 0; it < max_t; ++it) {
        double level = 0.5 * (lo + hi);
        double total = 0.0;
        for (auto& tank : tanks) {
            total += tank.volume(level);
        }
        if (sign(total - V) > 0) {
            hi = level;
        } else {
            lo = level;
        }
    }
    return 0.5 * (lo + hi);
}

int main() {
    init();
    int N = read<int>();
    int V = read<int>();
    tanks tanks(N, tank { 0, 0 });
    for (int i = 0; i < N; ++i) {
        tanks[i].c = read<int>();
        tanks[i].r = read<int>();
    }
    double level = fill(tanks, V);
    for (auto& tank : tanks) {
        cout << tank.height(level) << endl;
    }
    return 0;
}
