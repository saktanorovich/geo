/**
 * Quadratic — solve a*x^2 + b*x + c = 0 over the integers, print LaTeX.
 *
 * Everything stays exact: the answer is a symbolic string, never a float, so
 * there is nothing to round and the checker matches tokens verbatim.
 *
 * Cases, in the order they are tested:
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
        ll t = a % b;
        a = b;
        b = t;
    }
    return a;
}

// Format a single rational value num/den as simplified LaTeX.
string frac(ll num, ll den) {
    if (den < 0) { den = -den; num = -num; }
    ll g = gcd(num, den);
    if (g == 0) g = 1;
    num /= g;
    den /= g;
    if (den == 1) return to_string(num);
    if (num < 0)
        return "-\\frac{" + to_string(-num) + "}{" + to_string(den) + "}";
    return "\\frac{" + to_string(num) + "}{" + to_string(den) + "}";
}

// Extract the largest square factor of n (n > 0): n = g*g*r, r square-free.
void simplify_sqrt(ll n, ll& g, ll& r) {
    g = 1;
    r = n;
    for (ll d = 2; d * d <= r; ++d) {
        while (r % (d * d) == 0) {
            r /= d * d;
            g *= d;
        }
    }
}

// Build the LaTeX for the conjugate pair (p +/- q*[i]sqrt(r)) / s.
// Used for irrational-real (imag=false) and complex (imag=true) roots.
string pm_expr(ll p, ll q, ll r, ll s, bool imag) {
    if (s < 0) { s = -s; p = -p; }
    ll g = gcd(gcd(p, q), s);
    if (g == 0) g = 1;
    p /= g;
    q /= g;
    s /= g;

    string surd = (r == 1) ? "" : "\\sqrt{" + to_string(r) + "}";
    string qstr;
    if (r == 1 && !imag) {
        qstr = to_string(q);                       // pure integer coefficient
    } else {
        qstr = (q == 1 ? "" : to_string(q)) + (imag ? "i" : "") + surd;
    }

    string num = (p == 0) ? "\\pm " + qstr
                          : to_string(p) + " \\pm " + qstr;
    if (s == 1) return num;
    return "\\frac{" + num + "}{" + to_string(s) + "}";
}

string solve(ll a, ll b, ll c) {
    if (a == 0) {
        if (b == 0) {
            if (c == 0) return "x \\in \\mathbb{R}";   // any x
            return "x \\in \\varnothing";              // no solutions
        }
        return "x = " + frac(-c, b);                    // linear
    }

    ll D = b * b - 4 * a * c;
    if (D == 0) {
        return "x = " + frac(-b, 2 * a);               // double root
    }

    if (D > 0) {
        ll g, r;
        simplify_sqrt(D, g, r);
        if (r == 1) {                                  // perfect square -> rational roots
            string x1 = frac(-b + g, 2 * a);
            string x2 = frac(-b - g, 2 * a);
            return "x_1 = " + x1 + ", \\quad x_2 = " + x2;
        }
        return "x_{1,2} = " + pm_expr(-b, g, r, 2 * a, false);
    }

    // D < 0 -> complex conjugate roots
    ll g, r;
    simplify_sqrt(-D, g, r);
    return "x_{1,2} = " + pm_expr(-b, g, r, 2 * a, true);
}

int main() {
    ios::sync_with_stdio(false);
    ll a, b, c;
    if (!(cin >> a >> b >> c)) return 0;
    cout << solve(a, b, c) << "\n";
    return 0;
}
