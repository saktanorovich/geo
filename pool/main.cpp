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

int sign(double x) {
    if (x + eps < 0) return -1;
    if (x - eps > 0) return +1;
    return 0;
}

double dist(double ax, double ay, double bx, double by) {
    return std::sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

struct triangle {
public:
    double catet;
    double hypot;
public:
    triangle() { };
    triangle(double catet, double hypot) {
        this->catet = catet;
        this->hypot = hypot;
    }
public:
    double square(double radius) const {
        if (sign(radius - hypot) >= 0) {
            return 0.5 * catet * std::sqrt(hypot * hypot - catet * catet);
        }
        if (sign(radius - catet) < 0) {
            return 0.5 * radius * radius * std::acos(catet / hypot);
        }
        return 0.5 * (
            catet * std::sqrt(radius * radius - catet * catet) +
                radius * radius * (
                    std::acos(catet / hypot) -
                    std::acos(catet / radius))
        );
    }
};

double square(vector<triangle>& triangles, double radius) {
    double res = 0.0;
    for (auto& triangle : triangles) {
        res += triangle.square(radius);
    }
    return res;
}

double fill(int n, int m, int x, int y, int v) {
    vector<triangle> triangles(8);
    triangles[0] = triangle(n - x, dist(n, 0, x, y));
    triangles[1] = triangle(n - x, dist(n, m, x, y));
    triangles[2] = triangle(m - y, dist(n, m, x, y));
    triangles[3] = triangle(m - y, dist(0, m, x, y));
    triangles[4] = triangle(x, dist(0, m, x, y));
    triangles[5] = triangle(x, dist(0, 0, x, y));
    triangles[6] = triangle(y, dist(0, 0, x, y));
    triangles[7] = triangle(y, dist(n, 0, x, y));
    double lo = 0.0, hi = +oo;
    for (auto _ = 0; _ < max_t; ++_) {
        double r = (lo + hi) * 0.5;
        if (sign(square(triangles, r) - v) > 0) {
            hi = r;
        } else {
            lo = r;
        }
    }
    return (lo + hi) * 0.5;
}

int main() {
    init();
    int n, m, x, y, v, t;
    cin >> n >> m;
    cin >> x >> y;
    cin >> v >> t;
    cout << fill(n, m, x, y, v * t) << endl;
    return 0;
}
