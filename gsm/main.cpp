#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

const double eps = 1e-10;
const double oo  = 1e+10;

inline void init() {
    ios::sync_with_stdio(false);
    cout.precision(10);
    cout.setf(ios::fixed);
}

int sign(double const& x) {
    if (x + eps < 0) return -1;
    if (x - eps > 0) return +1;
    return 0;
}

struct point {
    double x, y;
    point() { }
    point(double const& x, double const& y) :
        x(x), y(y) {
    }
};

struct segment {
    point p1, p2;
    segment(point const& p1, point const& p2) {
        this->p1 = p1;
        this->p2 = p2;
    }
};

struct zone {
    point center;
    double radius;
};

double dist(point const& a, point const& b) {
    return std::sqrt(
        (a.x - b.x) * (a.x - b.x) +
        (a.y - b.y) * (a.y - b.y));
}

bool contains_inner(zone const& z, point const& p) {
    double d = dist(z.center, p);
    if (sign(d - z.radius) < 0)
        return true;
    else
        return false;
}

bool contains_outer(zone const& z, point const& p) {
    double d = dist(z.center, p);
    if (sign(d - z.radius) == 0)
        return true;
    else
        return false;
}

bool contains(zone const& z, segment const& s) {
    double d1 = dist(z.center, s.p1);
    double d2 = dist(z.center, s.p2);
    return (
        sign(d1 - z.radius) <= 0 &&
        sign(d2 - z.radius) <= 0
    );
}

segment* intersect(zone const& z, segment const& s) {
    double x1 = s.p1.x, y1 = s.p1.y;
    double x2 = s.p2.x, y2 = s.p2.y;
    double dx = x2 - x1;
    double dy = y2 - y1;
    double a = dx * dx + dy * dy;
    double b = 2 * (dx * (x1 - z.center.x) + dy * (y1 - z.center.y));
    double c = (x1 - z.center.x) * (x1 - z.center.x) +
               (y1 - z.center.y) * (y1 - z.center.y) -
               z.radius * z.radius;
    double d = b * b - 4 * a * c;
    if (d < 0) {
        return nullptr;
    }
    vector<point> pts;
    double u1 = (-b - sqrt(d)) / (2 * a);
    double u2 = (-b + sqrt(d)) / (2 * a);
    if (0 <= u1 && u1 <= 1) pts.push_back(point{ x1 + u1 * dx, y1 + u1 * dy });
    if (0 <= u2 && u2 <= 1) pts.push_back(point{ x1 + u2 * dx, y1 + u2 * dy });
    if (pts.size() == 2) {
        return new segment(pts.front(), pts.back());
    }
    return nullptr;
}

vector<point> intersect(zone const& z1, zone const& z2) {
    double d = dist(z1.center, z2.center);
    double a = abs(z1.radius - z2.radius);
    double b = abs(z1.radius + z2.radius);
    vector<point> points;
    if (sign(a - d) < 0 && sign(d - b) <= 0) {
        double r = (z1.radius * z1.radius - z2.radius * z2.radius + d * d) / (2 * d);
        double cos = (z2.center.x - z1.center.x) / d;
        double sin = (z2.center.y - z1.center.y) / d;
        if (sign(d - b) < 0) {
            double h = sqrt(z1.radius * z1.radius - r * r);
            points.push_back(point(
                z1.center.x + (r * cos - (+h) * sin),
                z1.center.y + (r * sin + (+h) * cos)));
            points.push_back(point(
                z1.center.x + (r * cos - (-h) * sin),
                z1.center.y + (r * sin + (-h) * cos)));
        } else {
            points.push_back(point(
                z1.center.x + r * cos,
                z1.center.y + r * sin));
        }
    }
    return points;
}

bool contains(vector<zone>& zones, segment const& s) {
    vector<point> points;
    points.push_back(s.p1);
    points.push_back(s.p2);
    for (auto& zone : zones) {
        segment* r = intersect(zone, s);
        if (r != nullptr) {
            points.push_back(r->p1);
            points.push_back(r->p2);
        }
    }
    sort(points.begin(), points.end(),
        [](point const& a, point const& b) {
            return a.x < b.x;
        }
    );
    int num_points = points.size();
    for (int i = 0; i + 1 < num_points; ++i) {
        segment s(points[i], points[i + 1]);
        if (!std::any_of(zones.begin(), zones.end(),
            [&](zone const& z) {
                return contains(z, s);
            }))
            return false;
    }
    return true;
}

double spfa(vector<vector<pair<int, double>>>& graph, int source, int target) {
    int num_vertices = graph.size();
    vector<double> best(num_vertices, +oo);
    best[source] = 0;
    vector<bool> has(num_vertices, false);
    queue<int> que;
    que.push(source);
    while (!que.empty()) {
        int curr = que.front(); que.pop();
        has[curr] = false;
        for (auto e : graph[curr]) {
            auto next = e.first;
            auto dist = e.second;
            auto esta = best[curr] + dist;
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

double process(vector<zone> &zones, point const& src, point const& dst) {
    vector<point> points;
    points.push_back(src);
    points.push_back(dst);
    int num_zones = zones.size();
    for (int i = 0; i < num_zones; ++i) {
        for (int j = i + 1; j < num_zones; ++j) {
            vector<point> pts = intersect(zones[i], zones[j]);
            for (auto& p : pts) {
                // x10 faster
                if (std::any_of(zones.begin(), zones.end(),
                    [&](zone const& z) {
                        return contains_inner(z, p);
                    })) {
                    continue;
                }
                points.push_back(p);
            }
        }
    }
    int num_vertices = points.size();
    vector<vector<pair<int, double>>> graph(num_vertices);
    for (int i = 0; i < num_vertices; ++i) {
        for (int j = i + 1; j < num_vertices; ++j) {
            segment s(points[i], points[j]);
            if (contains(zones, s)) {
                double d = dist(s.p1, s.p2);
                graph[i].push_back({ j, d });
                graph[j].push_back({ i, d });
            }
        }
    }
    return spfa(graph, 0, 1);
}

int main() {
    init();
    point a, b;
    int nzones;
    scanf("%lf %lf", &a.x, &a.y);
    scanf("%lf %lf", &b.x, &b.y);
    scanf("%d", &nzones);
    vector<zone> zones;
    for (int i = 0; i < nzones; ++i) {
        zone z;
        scanf("%lf %lf %lf", &z.center.x, &z.center.y, &z.radius);
        zones.push_back(z);
    }
    printf("%.10lf", process(zones, a, b));
    return 0;
}
