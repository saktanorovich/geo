#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

const double eps = 1e-6;
const double oo  = 1e+6;
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
    int r;
    int h;
public:
    double volume(const double& level) const {
        return 0;
    }
    double height(const double &level) const {
        return 0;
    }
};
typedef vector<tank> tanks;

double fill(const tanks& tanks, int V) {
    double lo = 0.0, hi = +oo;
    for (auto _ = 0; _ < max_t; ++_) {
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
        tanks[i].h = read<int>();
        tanks[i].r = read<int>();
    }
    double level = fill(tanks, V);
    for (auto& tank : tanks) {
        cout << tank.height(level) << endl;
    }
    return 0;
}
