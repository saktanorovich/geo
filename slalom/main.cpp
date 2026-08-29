/**
 * Slalom — shortest run through the gates, in the given order.
 *
 * Start at (0,0), finish at (X,Y), and pass N <= 5000 horizontal gates, gate i
 * spanning (Ai, Yi) to (Bi, Yi) with 0 < Y1 < ... < YN < Y. Riding along a post
 * counts as passing. Print the minimum distance to 1e-3.
 *
 * The taut string: the shortest run is the straight start-to-finish segment
 * pulled tight against the posts blocking it, so it is a polyline that bends
 * only at posts, and only at the post that constrains it most.
 *
 * run(p0, p1, s..f) scans gates s..f once, keeping the left post lying furthest
 * to the left of the ray p0->p1 and the right post lying furthest to its right.
 * If no post is on the wrong side, the straight segment already clears every
 * gate and its length is the answer. Otherwise the taut path must touch that
 * extreme post, so the run is split there and recursion handles both halves,
 * each over a strictly smaller range of gates.
 *
 * Orientation is decided by an integer cross product, so no epsilon is needed:
 * with |coordinates| <= 20000 and every Yi >= 0, each term stays under 8e8 and
 * their difference under 1.6e9, inside int.
 */

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
