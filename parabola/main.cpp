#include <cmath>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

const double phi = 2 - (1 + sqrt(5)) / 2;
const double eps = 1e-10;
const double oo  = 1e+10;
const int max_t  = 50;

inline void init() {
    ios::sync_with_stdio(false);
    cout.precision(10);
    cout.setf(ios::fixed);
}

int sign(double x) {
    if (x + eps < 0) return -1;
    if (x - eps > 0) return +1;
    return 0;
}

struct point {
    double x, y;
};

double dist(point const& a, point const& b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

struct parabola {
private:
    inline double eval(double x) const {
        return a * x * x + b * x + c;
    }
    template<class F> double gold(F func, double xl, double xr) const {
        double lo0 = xl;
        double hi0 = xr;
        double lo3 = lo0 + phi * (hi0 - lo0);
        double hi3 = hi0 - phi * (hi0 - lo0);
        double flo = func(lo3);
        double fhi = func(hi3);
        for (int it = 0; it < max_t; ++it) {
            if (flo - eps > fhi) {
                lo0 = lo3;
                lo3 = hi3;
                hi3 = hi0 - phi * (hi0 - lo0);
                flo = fhi;
                fhi = func(hi3);
            } else {
                hi0 = hi3;
                hi3 = lo3;
                lo3 = lo0 + phi * (hi0 - lo0);
                fhi = flo;
                flo = func(lo3);
            }
        }
        return func(0.5 * (lo0 + hi0));
    }
    double minimum(point const& p, double xl, double xr) const {
        auto f = [&](double x) {
            return dist(p, point{ x, eval(x) });
        };
        return gold(f, xl, xr);
    }
    double minimum(parabola const& p, double xl, double xr) const {
        auto f = [&](double x) {
            return p.minimum(point{ x, eval(x) });
        };
        return gold(f, xl, xr);
    }
public:
    double a, b, c;
    double xl, xr;
public:
    template<class P> double minimum(P const& p) const {
        double x = -b / 2 / a;
        if (sign(xl - x) < 0 && sign(x - xr) < 0) {
            double d1 = minimum(p, xl, x);
            double d2 = minimum(p, x, xr);
            if (d1 + eps < d2)
                return d1;
            else
                return d2;
        }
        return minimum(p, xl, xr);
    }
};

double spfa(vector<vector<double>>& graph, int source, int target) {
    int num_vertices = graph.size();
    vector<double> best(num_vertices, +oo);
    best[source] = 0;
    vector<bool> has(num_vertices, false);
    queue<int> que;
    que.push(source);
    while (!que.empty()) {
        int curr = que.front(); que.pop();
        has[curr] = false;
        for (int next = 0; next < num_vertices; ++next) {
            double esta = best[curr] + graph[curr][next];
            if (best[next] > esta + eps) {
                best[next] = esta;
                if (has[next] == false) {
                    has[next] = true;
                    que.push(next);
                }
            }
        }
    }
    return best[target];
}

double process(vector<parabola>& parabolas, point const& source, point const& target) {
    int num_parabolas = parabolas.size();
    if (num_parabolas < 1) {
      return dist(source, target);
    }
    int num_vertices = num_parabolas + 2;
    vector<vector<double> > graph(num_vertices, vector<double>(num_vertices, +oo));
    graph[0][1] = dist(source, target);
    graph[1][0] = dist(source, target);
    for (int i = 0; i < num_parabolas; ++i) {
        graph[0][i + 2] = graph[i + 2][0] = parabolas[i].minimum(source);
        graph[1][i + 2] = graph[i + 2][1] = parabolas[i].minimum(target);
    }
    for (int i = 0; i < num_parabolas; ++i) {
        for (int j = i + 1; j < num_parabolas; ++j) {
            double d = parabolas[i].minimum(parabolas[j]);
            graph[i + 2][j + 2] = d;
            graph[j + 2][i + 2] = d;
        }
    }
    for (int i = 0; i < num_vertices; ++i) {
        graph[i][i] = 0;
    }
    return spfa(graph, 0, 1);
}

int main() {
    init();
    int num_parabolas;
    point source, target;
    cin >> num_parabolas;
    cin >> source.x >> source.y;
    cin >> target.x >> target.y;
    vector<parabola> parabolas;
    for (int i = 0; i < num_parabolas; ++i) {
        parabola teleport;
        cin
            >> teleport.a
            >> teleport.b
            >> teleport.c
            >> teleport.xl
            >> teleport.xr;
        parabolas.push_back(teleport);
    }
    cout << process(parabolas, source, target) << endl;
    return 0;
}
