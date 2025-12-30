#include <cmath>
#include <iostream>
using namespace std;

inline void init() {
    ios::sync_with_stdio(false);
    cout.precision(4);
    cout.setf(ios::fixed);
}

int xl[5000], xr[5000], y[5000];
int n, fx, fy;

int vector_product(int x1, int y1, int x2, int y2) {
    return x1 * y2 - x2 * y1;
}

double run(int x0, int y0, int x1, int y1, int s, int f) {
    int lx = x1, ly = y1;
    int rx = x1, ry = y1;
    int l = -1;
    int r = -1;

    for (int i = s; i <= f; ++i) {
        if (vector_product(lx - x0, ly - y0, xl[i] - x0, y[i] - y0) <= 0) {
            lx = xl[i];
            ly = y[i];
            l = i;
        }
        if (vector_product(rx - x0, ry - y0, xr[i] - x0, y[i] - y0) >= 0) {
            rx = xr[i];
            ry = y[i];
            r = i;
        }
    }

    if (l != -1) return run(x0, y0, lx, ly, s, l - 1) + run(lx, ly, x1, y1, l + 1, f);
    if (r != -1) return run(x0, y0, rx, ry, s, r - 1) + run(rx, ry, x1, y1, r + 1, f);

    return sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

int main() {
    init();
    cin >> n >> fx >> fy;
    for (int i = 0; i < n; ++i) {
        cin >> xl[i] >> xr[i] >> y[i];
    }
    cout << run(0, 0, fx, fy, 0, n - 1) << endl;
    return 0;
}
