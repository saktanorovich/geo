/**
 * Triopt — optimize F1(x) + F2(y) + F3(z) subject to x + y + z = S.
 *
 * x, y, z and S are natural numbers, 3 <= S <= 1e6, each Fi is sin or cos, and
 * either min or max is asked for. Print the optimum to 1e-10. Brute force over
 * pairs is O(S^2), 1e12 at the limit, so the pair has to collapse.
 *
 * Two of the three functions are always the same one. Call the odd one out
 * `main_func` and the repeated one `help_func`; fix x, so y + z = total = S - x
 * is fixed too, and a sum-to-product identity leaves a single free term:
 *   sin y + sin z = 2 * sin(total/2) * cos((y - z)/2)
 *   cos y + cos z = 2 * cos(total/2) * cos((y - z)/2)
 * that is, 2 * help_func(total/2) * cos(d/2) with only d = y - z still free.
 * For integers y, z >= 1 the difference d has the parity of total and satisfies
 * |d| <= total - 2, and cos is even, so the reachable factors are exactly
 * cos(d/2) for d = total-2, total-4, ... down to 0 or 1.
 *
 * max_cache[t] and min_cache[t] hold the largest and smallest cos(d/2) over
 * that set, each derived in O(1) from the entry two steps back. help_func may
 * be negative, so both extremes are handed to the optimizer, which keeps the
 * one that suits min or max. The answer is then a single O(S) sweep over x.
 *
 * The two caches are sized for the S limit up front: 8 MB each.
 */

#include <iostream>
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
using namespace std;

const int max_s = 1000000 + 1;

inline void init() {
    ios::sync_with_stdio(false);
    cout.precision(15);
    cout.setf(ios::fixed);
}

class optimizer {
public:
    virtual double defaulte() = 0;
    virtual double comparee(double a, double b) = 0;
};

class maximizer : public optimizer {
public:
    double defaulte() override {
        return -1e+15;
    }
    double comparee(double a, double b) override {
        if (a - 1e-15 > b)
            return a;
        else
            return b;
    }
};

class minimizer : public optimizer {
public:
    double defaulte() override {
      return +1e+15;
    }
    double comparee(double a, double b) override {
        if (a + 1e-15 < b)
            return a;
        else
            return b;
    }
};

typedef double (*trigonometry)(double x);

class expression {
public:
    trigonometry main_func;
    trigonometry help_func;
public:
    expression() : main_func(nullptr), help_func(nullptr) {}
    expression(trigonometry main_func, trigonometry help_func) {
        this->main_func = main_func;
        this->help_func = help_func;
    }
};

maximizer maxx_inst;
minimizer minn_inst;
optimizer* maxx = &maxx_inst;
optimizer* minn = &minn_inst;
double max_cache[max_s];
double min_cache[max_s];

unique_ptr<optimizer> parse(string const& name) {
    if (name == "max") return make_unique<maximizer>();
    if (name == "min") return make_unique<minimizer>();
    return nullptr;
}

double optimize(expression const& expression, optimizer* optimizer, int summa) {
    double result = optimizer->defaulte();
    for (int x = 1; x <= summa - 2; ++x) {
        int total = summa - x;
        double help_max = 2 * expression.help_func(total / 2.0) * max_cache[total - 2];
        double help_min = 2 * expression.help_func(total / 2.0) * min_cache[total - 2];
        result = optimizer->comparee(result, expression.main_func(x) + optimizer->comparee(help_max, help_min));
    }
    return result;
}

int main() {
    init();
    unordered_map<string, expression> map;
    map["coscoscos"] = expression(cos, cos);
    map["coscossin"] = expression(sin, cos);
    map["cossincos"] = expression(sin, cos);
    map["cossinsin"] = expression(cos, sin);
    map["sincoscos"] = expression(sin, cos);
    map["sincossin"] = expression(cos, sin);
    map["sinsincos"] = expression(cos, sin);
    map["sinsinsin"] = expression(sin, sin);
    for (int total = 0; total < max_s; ++total) {
        max_cache[total] = cos(total / 2.0);
        min_cache[total] = cos(total / 2.0);
        if (total > 1) {
            max_cache[total] = maxx->comparee(max_cache[total], max_cache[total - 2]);
            min_cache[total] = minn->comparee(min_cache[total], min_cache[total - 2]);
        }
    }
    string trig[3];
    cin >> trig[0] >> trig[1] >> trig[2];
    auto goal = string();
    auto suma = 0;
    cin >> goal;
    cin >> suma;
    auto opt = parse(goal);
    cout << optimize(map[trig[0] + trig[1] + trig[2]], opt.get(), suma) << endl;
    return 0;
}
