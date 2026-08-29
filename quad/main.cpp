/**
 * Quadratic — solve a*x^2 + b*x + c = 0 over the integers, print the roots as
 * LaTeX inside a Markdown math span.
 *
 * Everything stays exact: the answer is a symbolic string, never a float, so
 * there is nothing to round and the checker matches tokens verbatim.
 *
 * Cases, in the order solver::solve tests them:
 *   a = 0, b = 0, c = 0   every x            -> x \in \mathbb{R}
 *   a = 0, b = 0, c != 0  no solution        -> x \in \varnothing
 *   a = 0                 linear             -> x = -c/b
 *   D = 0                 double root        -> x = -b/(2a)
 *   D > 0, D a square     two rationals      -> x_1, x_2
 *   D > 0                 conjugate surds    -> x_{1,2} = (-b +- g*sqrt r)/(2a)
 *   D < 0                 complex conjugates -> the same, with i
 *
 * D = b^2 - 4ac is computed in long long. Output is fully reduced: fractions
 * divided through by their gcd, square factors pulled out of the surd (D is
 * split as g^2 * r with r square-free, by trial division up to sqrt D), and the
 * trivial parts — a coefficient of 1, a denominator of 1, a zero real part —
 * left out. All literals are ASCII, so nothing depends on output encoding.
 *
 * latex::render wraps the finished formula in $...$, so a line of output drops
 * straight into Markdown and renders as maths. The expected outputs under
 * tests/ are .md for the same reason: check.py ./quad --str --out md.
 */

#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

typedef long long ll;

ll gcd(ll a, ll b) {
    a = llabs(a);
    b = llabs(b);
    while (b) {
        ll remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

struct fraction {
    ll numerator;
    ll denominator;

    fraction(ll top = 0, ll bottom = 1) : numerator(top), denominator(bottom) {
        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
        ll common = gcd(numerator, denominator);
        if (common > 1) {
            numerator /= common;
            denominator /= common;
        }
    }
};

struct radical {
    ll factor;
    ll radicand;

    explicit radical(ll n = 1) : factor(1), radicand(n) {
        for (ll d = 2; d * d <= radicand; ++d) {
            while (radicand % (d * d) == 0) {
                radicand /= d * d;
                factor *= d;
            }
        }
    }
};

struct conjugate_pair {
    ll offset;
    radical root;
    ll denominator;
    bool imaginary;

    conjugate_pair(ll shift = 0, radical surd = radical(), ll scale = 1, bool complex_valued = false)
        : offset(shift), root(surd), denominator(scale), imaginary(complex_valued) {
        if (denominator < 0) {
            offset = -offset;
            denominator = -denominator;
        }
        ll common = gcd(gcd(offset, root.factor), denominator);
        offset /= common;
        root.factor /= common;
        denominator /= common;
    }
};

enum solution_kind {
    all_reals,
    no_roots,
    single_root,
    rational_roots,
    conjugate_roots
};

struct solution {
    solution_kind kind = all_reals;
    fraction first;
    fraction second;
    conjugate_pair pair;
};

class solver {
public:
    solution solve(ll a, ll b, ll c) const {
        solution answer;

        if (a == 0 && b == 0) {
            answer.kind = c == 0 ? all_reals : no_roots;
            return answer;
        }

        if (a == 0) {
            answer.kind = single_root;
            answer.first = fraction(-c, b);
            return answer;
        }

        ll discriminant = b * b - 4 * a * c;
        if (discriminant == 0) {
            answer.kind = single_root;
            answer.first = fraction(-b, 2 * a);
            return answer;
        }

        radical surd(llabs(discriminant));

        if (discriminant > 0 && surd.radicand == 1) {
            answer.kind = rational_roots;
            answer.first = fraction(-b + surd.factor, 2 * a);
            answer.second = fraction(-b - surd.factor, 2 * a);
            return answer;
        }

        answer.kind = conjugate_roots;
        answer.pair = conjugate_pair(-b, surd, 2 * a, discriminant < 0);
        return answer;
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
            case all_reals:      return "x \\in \\mathbb{R}";
            case no_roots:       return "x \\in \\varnothing";
            case single_root:    return "x = " + render(answer.first);
            case rational_roots: return "x_1 = " + render(answer.first) +
                                        ", \\quad x_2 = " + render(answer.second);
            case conjugate_roots: return "x_{1,2} = " + render(answer.pair);
        }
        return "";
    }

    string render(const fraction& value) const {
        if (value.denominator == 1) return integer(value.numerator);
        if (value.numerator < 0) return "-" + ratio(integer(-value.numerator), integer(value.denominator));
        return ratio(integer(value.numerator), integer(value.denominator));
    }

    string render(const conjugate_pair& roots) const {
        string spread = "\\pm " + term(roots);
        string numerator = roots.offset == 0 ? spread : integer(roots.offset) + " " + spread;
        if (roots.denominator == 1) return numerator;
        return ratio(numerator, integer(roots.denominator));
    }

    string term(const conjugate_pair& roots) const {
        if (roots.root.radicand == 1 && !roots.imaginary) return integer(roots.root.factor);
        string coefficient = roots.root.factor == 1 ? "" : integer(roots.root.factor);
        string unit = roots.imaginary ? "i" : "";
        string surd = roots.root.radicand == 1 ? "" : "\\sqrt{" + integer(roots.root.radicand) + "}";
        return coefficient + unit + surd;
    }

    string integer(ll value) const {
        return to_string(value);
    }

    string math(const string& body) const {
        return "$" + body + "$";
    }

    string ratio(const string& numerator, const string& denominator) const {
        return "\\frac{" + numerator + "}{" + denominator + "}";
    }
};

void init() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
}

int main() {
    init();
    ll a = 0, b = 0, c = 0;
    cin >> a >> b >> c;
    solver solver;
    solution solution = solver.solve(a, b, c);
    latex tex;
    string result = tex.render(solution);
    cout << result;
    return 0;
}
