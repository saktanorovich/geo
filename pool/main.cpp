/**
 * Pool — how far the spilled water reaches.
 *
 * The container is the rectangle (0,0)-(n,m); v cubic metres a minute are
 * poured at (x,y) for t minutes and spread out uniformly, so the wetted region
 * is the disk of radius r about (x,y) clipped by the four walls. Depth is unit
 * and v*t <= n*m, so r is pinned by "clipped area == v*t", and the farthest
 * point the water reaches is exactly r away. Print r to 1e-3.
 *
 * Clipped area in closed form: drop a perpendicular from (x,y) to each wall and
 * a diagonal to each corner. That cuts the rectangle into 8 right triangles,
 * each having the pour point as a vertex, a leg `catet` (distance to its wall)
 * and a hypotenuse `hypot` (distance to its corner). Within one triangle:
 *   r >= hypot   the whole triangle is wet,
 *   r <  catet   a circular sector, the wall is still out of reach,
 *   otherwise    the triangle up to the wall plus the leftover sector.
 *
 * The total is continuous and increasing in r, so 40 bisection steps on
 * [0, 1e6] pin it down far below the required tolerance.
 */

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
