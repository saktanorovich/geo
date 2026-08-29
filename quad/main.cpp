/**
 * Quadratic — solve a*x^2 + b*x + c = 0 over the integers, print the roots as
 * LaTeX inside a Markdown math span.
 *
 * Everything stays exact: the answer is a symbolic string, never a float.
 *
 * Cases:
 *   a = 0, b = 0, c = 0   every x             -> x \in \mathbb{C}
 *   a = 0, b = 0, c != 0  no solution         -> x \in \varnothing
 *   a = 0                  linear              -> x = -c/b
 *   D = 0                  double root         -> x = -b/(2a)
 *   D > 0, D a square      two rationals       -> x_1, x_2
 *   D > 0                  conjugate surds     -> x_{1,2} = (-b +- g*sqrt(r))/(2a)
 *   D < 0                  complex conjugates  -> same, with i
 */

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

typedef long long ll;

void init() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
}

ll gcd(ll a, ll b) {
    a = llabs(a);
    b = llabs(b);

    while (b) {
        ll t = a % b;
        a = b;
        b = t;
    }

    return a;
}

struct fraction {
public:
    ll num;
    ll den;

public:
    fraction(ll num = 0, ll den = 1)
        : num(num), den(den) {

        if (den < 0) {
            this->num = -this->num;
            this->den = -this->den;
        }

        ll d = gcd(this->num, this->den);

        if (d > 1) {
            this->num /= d;
            this->den /= d;
        }
    }
};

struct radical {
public:
    ll factor;
    ll number;

public:
    radical(ll fac, ll num)
        : factor(fac), number(num) {
    }

    radical(ll num = 1)
        : factor(1), number(num) {

        for (ll d = 2; d <= number / d; ++d) {
            while (number % (d * d) == 0) {
                number /= d * d;
                factor *= d;
            }
        }
    }

    radical& operator/=(ll d) {
        factor /= d;
        return *this;
    }
};

using rd = radical;

struct conjugate_nums {
public:
    ll off;
    ll div;
    rd roo;
    int im;

public:
    conjugate_nums()
        : off(0), div(1), roo(), im(0) {
    }

    conjugate_nums(ll off, ll div, radical roo, int im)
        : off(off), div(div), roo(roo), im(im) {

        // Keep denominator positive.
        if (this->div < 0) {
            this->div = -this->div;
            this->off = -this->off;
        }

        // Reduce every term in
        //
        //     off +- factor*sqrt(number)
        //     --------------------------
        //                div
        //
        // by their common divisor.
        ll d = gcd(
            gcd(this->off, this->div),
            this->roo.factor
        );

        if (d > 1) {
            this->off /= d;
            this->div /= d;
            this->roo /= d;
        }
    }
};

enum solution_kind {
    all_numbers,
    no_solution,
    single_root,
    r_roots,
    c_roots
};

struct solution {
    solution_kind kind = all_numbers;
    fraction sol1;
    fraction sol2;
    conjugate_nums nums;
};

class solver {
public:
    solution solve(ll a, ll b, ll c) const {
        solution result;

        // 0*x^2 + 0*x + c = 0
        if (a == 0 && b == 0) {
            result.kind = (c == 0)
                ? all_numbers
                : no_solution;

            return result;
        }

        // 0*x^2 + b*x + c = 0
        if (a == 0) {
            result.kind = single_root;
            result.sol1 = fraction(-c, b);
            return result;
        }

        // a*x^2 + b*x + c = 0
        ll D = b * b - 4 * a * c;

        // One repeated rational root.
        if (D == 0) {
            result.kind = single_root;
            result.sol1 = fraction(-b, 2 * a);
            return result;
        }

        radical surd(llabs(D));

        // Positive perfect-square discriminant:
        // both roots are rational.
        if (D > 0 && surd.number == 1) {
            result.kind = r_roots;

            result.sol1 = fraction(
                -b + surd.factor,
                2 * a
            );

            result.sol2 = fraction(
                -b - surd.factor,
                2 * a
            );

            return result;
        }

        // Irrational real roots or complex conjugate roots.
        result.kind = c_roots;

        result.nums = conjugate_nums(
            -b,
            2 * a,
            surd,
            D < 0
        );

        return result;
    }
};

class latex {
public:
    string render(const solution& answer) const {
        return math(formula(answer));
    }
private:
    string formula(const solution& answer) const {
        switch (answer.kind) {
            case all_numbers:
                return "x \\in \\mathbb{C}";

            case no_solution:
                return "x \\in \\varnothing";

            case single_root:
                return "x = " + render(answer.sol1);

            case r_roots:
                return
                    "x_1 = " + render(answer.sol1) +
                    ", \\quad x_2 = " + render(answer.sol2);

            case c_roots:
                return "x_{1,2} = " + render(answer.nums);
        }

        return "";
    }

    string render(const fraction& f) const {
        if (f.den == 1) {
            return integer(f.num);
        }

        if (f.num < 0) {
            return "-" + ratio(
                integer(-f.num),
                integer(f.den)
            );
        }

        return ratio(
            integer(f.num),
            integer(f.den)
        );
    }

    string render(const conjugate_nums& nums) const {
        string root = term(nums);

        // No real part:
        //
        //     +-sqrt(...)
        //     -----------
        //          d
        if (nums.off == 0) {
            string numerator = "\\pm " + root;

            if (nums.div == 1) {
                return numerator;
            }

            return ratio(
                numerator,
                integer(nums.div)
            );
        }

        string numerator =
            integer(nums.off) +
            " \\pm " +
            root;

        if (nums.div == 1) {
            return numerator;
        }

        return ratio(
            numerator,
            integer(nums.div)
        );
    }

    string term(const conjugate_nums& roots) const {
        // Pure integer radical after simplification.
        if (roots.roo.number == 1 && !roots.im) {
            return integer(roots.roo.factor);
        }

        string coef =
            roots.roo.factor == 1
                ? ""
                : integer(roots.roo.factor);

        string surd =
            roots.roo.number == 1
                ? ""
                : "\\sqrt{" + integer(roots.roo.number) + "}";

        string unit = roots.im ? "i" : "";

        return coef + unit + surd;
    }

    string integer(ll value) const {
        return to_string(value);
    }

    string ratio(const string& num, const string& den) const {
        return "\\frac{" + num + "}{" + den + "}";
    }

    string math(const string& body) const {
        return "$" + body + "$";
    }
};

int main() {
    init();

    auto solver = make_unique<::solver>();
    auto writer = make_unique<::latex>();

    for (ll a, b, c; cin >> a >> b >> c;) {
        auto s = solver->solve(a, b, c);
        auto r = writer->render(s);

        cout << r;
        break;
    }

    return 0;
}