#include <iostream>
#include <cmath>
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
    expression(trigonometry main_func, trigonometry help_func) {
        this->main_func = main_func;
        this->help_func = help_func;
    }
};

optimizer *maxx = new maximizer();
optimizer *minn = new minimizer();
double max_cache[max_s];
double min_cache[max_s];

optimizer* parse(string const& name) {
    if (name == "max")
        return new maximizer();
    if (name == "min")
        return new minimizer();
    return nullptr;
}

double optimize(expression* expression, optimizer* optimizer, int summa) {
    double result = optimizer->defaulte();
    for (int x = 1; x <= summa - 2; ++x) {
        int total = summa - x;
        double help_max = 2 * expression->help_func(total / 2.0) * max_cache[total - 2];
        double help_min = 2 * expression->help_func(total / 2.0) * min_cache[total - 2];
        result = optimizer->comparee(result, expression->main_func(x) + optimizer->comparee(help_max, help_min));
    }
    return result;
}

int main() {
    init();
    unordered_map<string, expression*> map;
    map["coscoscos"] = new expression(cos, cos);
    map["coscossin"] = new expression(sin, cos);
    map["cossincos"] = new expression(sin, cos);
    map["cossinsin"] = new expression(cos, sin);
    map["sincoscos"] = new expression(sin, cos);
    map["sincossin"] = new expression(cos, sin);
    map["sinsincos"] = new expression(cos, sin);
    map["sinsinsin"] = new expression(sin, sin);
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
    cout << optimize(map[trig[0] + trig[1] + trig[2]], parse(goal), suma) << endl;
    return 0;
}
