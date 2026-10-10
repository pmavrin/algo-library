#ifdef int
#define MOD_LIBRARY_RESTORE_INT_MACRO
#pragma push_macro("int")
#undef int
#endif

#ifdef long
#define MOD_LIBRARY_RESTORE_LONG_MACRO
#pragma push_macro("long")
#undef long
#endif

#include <bits/stdc++.h>

#ifdef MOD_LIBRARY_RESTORE_LONG_MACRO
#pragma pop_macro("long")
#undef MOD_LIBRARY_RESTORE_LONG_MACRO
#endif

#ifdef MOD_LIBRARY_RESTORE_INT_MACRO
#pragma pop_macro("int")
#undef MOD_LIBRARY_RESTORE_INT_MACRO
#endif

using namespace std;

// 998244353 = 119 * 2^23 + 1; primitive root = 3.
// Therefore NTT supports transforms of size at most 2^23.
constexpr int MOD = 998244353;
constexpr int PRIMITIVE_ROOT = 3;

// Main API:
//   mint                         modular number;
//   power(x, e), inv(x)          power and multiplicative inverse;
//   fact(n), inv_fact(n), C(n,k), P(n,k);
//   comb.multinomial(parts);
//   convolution(a, b) / conv(a,b) for vector<mint> polynomials.

struct mint {
    int value;

    mint() : value(0) {}
    mint(int64_t x) {
        x %= MOD;
        if (x < 0) x += MOD;
        value = static_cast<int>(x);
    }

    static mint raw(int x) {
        mint result;
        result.value = x;
        return result;
    }

    explicit operator int() const { return value; }

    mint &operator+=(const mint &other) {
        value += other.value;
        if (value >= MOD) value -= MOD;
        return *this;
    }

    mint &operator-=(const mint &other) {
        value -= other.value;
        if (value < 0) value += MOD;
        return *this;
    }

    mint &operator*=(const mint &other) {
        value = static_cast<int>(int64_t(value) * other.value % MOD);
        return *this;
    }

    mint pow(int64_t exponent) const {
        assert(exponent >= 0);
        mint base = *this;
        mint result = 1;
        while (exponent > 0) {
            if (exponent & 1) result *= base;
            base *= base;
            exponent >>= 1;
        }
        return result;
    }

    mint inv() const {
        assert(value != 0);
        return pow(MOD - 2);
    }

    mint &operator/=(const mint &other) { return *this *= other.inv(); }

    mint operator+() const { return *this; }
    mint operator-() const { return value == 0 ? *this : raw(MOD - value); }

    friend mint operator+(mint left, const mint &right) { return left += right; }
    friend mint operator-(mint left, const mint &right) { return left -= right; }
    friend mint operator*(mint left, const mint &right) { return left *= right; }
    friend mint operator/(mint left, const mint &right) { return left /= right; }

    friend bool operator==(const mint &left, const mint &right) {
        return left.value == right.value;
    }
    friend bool operator!=(const mint &left, const mint &right) {
        return !(left == right);
    }

    friend ostream &operator<<(ostream &out, const mint &x) {
        return out << x.value;
    }
    friend istream &operator>>(istream &in, mint &x) {
        int64_t input;
        in >> input;
        x = mint(input);
        return in;
    }
};

mint power(mint base, int64_t exponent) { return base.pow(exponent); }
mint inv(mint x) { return x.inv(); }

struct Combinatorics {
    vector<mint> fac{1};
    vector<mint> ifac{1};

    void ensure(int n) {
        assert(0 <= n && n < MOD);
        if (n < static_cast<int>(fac.size())) return;

        int old_size = static_cast<int>(fac.size());
        fac.resize(n + 1);
        ifac.resize(n + 1);

        for (int i = old_size; i <= n; ++i) fac[i] = fac[i - 1] * i;
        ifac[n] = fac[n].inv();
        for (int i = n; i >= old_size; --i) ifac[i - 1] = ifac[i] * i;
    }

    mint factorial(int n) {
        if (n < 0) return 0;
        ensure(n);
        return fac[n];
    }

    mint inverse_factorial(int n) {
        if (n < 0) return 0;
        ensure(n);
        return ifac[n];
    }

    // Binomial coefficient: n choose k.
    mint C(int n, int k) {
        if (n < 0 || k < 0 || k > n) return 0;
        ensure(n);
        return fac[n] * ifac[k] * ifac[n - k];
    }

    // Number of ordered selections of k distinct elements from n.
    mint P(int n, int k) {
        if (n < 0 || k < 0 || k > n) return 0;
        ensure(n);
        return fac[n] * ifac[n - k];
    }

    // (a_1 + ... + a_m)! / (a_1! ... a_m!).
    mint multinomial(const vector<int> &parts) {
        int total = 0;
        for (int x : parts) {
            if (x < 0) return 0;
            total += x;
        }
        ensure(total);
        mint result = fac[total];
        for (int x : parts) result *= ifac[x];
        return result;
    }
};

inline Combinatorics comb;

void precalc(int n) { comb.ensure(n); }
void ensure_fact(int n) { comb.ensure(n); }
mint fact(int n) { return comb.factorial(n); }
mint inv_fact(int n) { return comb.inverse_factorial(n); }
mint C(int n, int k) { return comb.C(n, k); }
mint P(int n, int k) { return comb.P(n, k); }
mint calc_c(int n, int k) { return comb.C(n, k); }

void ntt(vector<mint> &a, bool inverse = false) {
    int n = static_cast<int>(a.size());
    if (n <= 1) return;
    assert((n & (n - 1)) == 0);
    assert((MOD - 1) % n == 0);

    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }

    for (int len = 2; len <= n; len <<= 1) {
        mint root = mint(PRIMITIVE_ROOT).pow((MOD - 1) / len);
        if (inverse) root = root.inv();

        for (int start = 0; start < n; start += len) {
            mint w = 1;
            for (int i = 0; i < len / 2; ++i) {
                mint even = a[start + i];
                mint odd = a[start + i + len / 2] * w;
                a[start + i] = even + odd;
                a[start + i + len / 2] = even - odd;
                w *= root;
            }
        }
    }

    if (inverse) {
        mint inverse_n = mint(n).inv();
        for (mint &x : a) x *= inverse_n;
    }
}

vector<mint> convolution(vector<mint> a, vector<mint> b) {
    if (a.empty() || b.empty()) return {};

    int result_size = static_cast<int>(a.size() + b.size() - 1);

    // The quadratic algorithm is faster for small polynomials.
    if (min(a.size(), b.size()) <= 32) {
        vector<mint> result(result_size);
        for (int i = 0; i < static_cast<int>(a.size()); ++i)
            for (int j = 0; j < static_cast<int>(b.size()); ++j)
                result[i + j] += a[i] * b[j];
        return result;
    }

    int n = 1;
    while (n < result_size) n <<= 1;
    assert(n <= (1 << 23));

    a.resize(n);
    b.resize(n);
    ntt(a);
    ntt(b);
    for (int i = 0; i < n; ++i) a[i] *= b[i];
    ntt(a, true);
    a.resize(result_size);
    return a;
}

vector<mint> conv(const vector<mint> &a, const vector<mint> &b) {
    return convolution(a, b);
}
