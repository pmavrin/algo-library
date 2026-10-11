#include <bits/stdc++.h>

#ifndef MATH_NO_INT_MACROS
#define int int64_t
#define long int64_t
#endif

#include "../math/mod.cpp"
#include "../math/poly.cpp"

poly slow_multiply(const poly &a, const poly &b, int n) {
    poly result(n);
    for (int i = 0; i < static_cast<int>(a.size()) && i < n; ++i) {
        for (int j = 0; j < static_cast<int>(b.size()) && i + j < n; ++j) {
            result[i + j] += a[i] * b[j];
        }
    }
    return result;
}

poly slow_inverse(const poly &a, int n) {
    poly result(n);
    if (n == 0) return result;
    result[0] = a[0].inv();
    for (int i = 1; i < n; ++i) {
        for (int j = 1; j <= i && j < static_cast<int>(a.size()); ++j) {
            result[i] -= a[j] * result[i - j];
        }
        result[i] *= result[0];
    }
    return result;
}

poly slow_exp(const poly &a, int n) {
    poly result(n);
    if (n == 0) return result;
    result[0] = 1;
    // If f = exp(a), then f' = a' * f.
    for (int i = 1; i < n; ++i) {
        for (int j = 1; j <= i && j < static_cast<int>(a.size()); ++j) {
            result[i] += mint(j) * a[j] * result[i - j];
        }
        result[i] /= mint(i);
    }
    return result;
}

poly slow_power(poly a, int64_t exponent, int n) {
    poly result(n);
    if (n == 0) return result;
    result[0] = 1;
    a.resize(n);
    while (exponent > 0) {
        if (exponent & 1) result = slow_multiply(result, a, n);
        a = slow_multiply(a, a, n);
        exponent >>= 1;
    }
    return result;
}

pair<poly, poly> slow_divide(poly a, poly b) {
    while (!a.empty() && a.back() == mint(0)) a.pop_back();
    while (!b.empty() && b.back() == mint(0)) b.pop_back();
    if (a.size() < b.size()) return {{}, a};
    poly quotient(a.size() - b.size() + 1);
    mint inverse = b.back().inv();
    for (int i = static_cast<int>(quotient.size()) - 1; i >= 0; --i) {
        quotient[i] = a[i + b.size() - 1] * inverse;
        for (size_t j = 0; j < b.size(); ++j) a[i + j] -= quotient[i] * b[j];
    }
    while (!a.empty() && a.back() == mint(0)) a.pop_back();
    return {quotient, a};
}

void test_special_cases() {
    assert(poly_add({1, 2}, {-1, -2}).empty());
    assert(poly_sub({}, {1}) == poly{mint(-1)});
    assert(poly_mul({}, {1}).empty());
    assert(poly_mul({0, 0}, {1, 2}).empty());
    assert(poly_evaluate({}, 12) == mint(0));
    assert(poly_evaluate({1, 2, 3}, 2) == mint(17));
    assert(poly_prefix({1, 2}, 4) == poly({1, 2, 0, 0}));
    assert(poly_derivative({7, 2, 3, 0}) == poly({2, 6}));
    assert(poly_integral({2, 6}) == poly({0, 2, 3}));
    assert(poly_inverse({}, 0).empty());
    assert(poly_log({}, 0).empty());
    assert(poly_exp({}, 0).empty());
    assert(poly_pow({}, 0, 0).empty());
    assert(poly_pow({}, 0, 4) == poly({1, 0, 0, 0}));
    assert(poly_pow({}, 3, 4) == poly(4));
    assert(poly_exp({}, 4) == poly({1, 0, 0, 0}));
    assert(poly_log({1}, 4) == poly(4));
    assert(poly_pow({0, 2}, 3, 5) == poly({0, 0, 0, 8, 0}));
    assert(poly_pow({0, 1}, numeric_limits<int64_t>::max(), 10) == poly(10));
    assert(poly_pow({1, 1}, MOD, 100) == poly_prefix({1}, 100));
    assert((poly_divmod({0, 0}, {1, 0}) == make_pair(poly{}, poly{})));
    assert((poly_divmod({1, 2, 1, 0}, {1, 1, 0}) == make_pair(poly({1, 1}), poly{})));

    const int n = 150;
    assert(poly_inverse({1, -1}, n) == poly(n, mint(1)));
    poly logarithm(n), exponential(n);
    exponential[0] = 1;
    for (int i = 1; i < n; ++i) {
        logarithm[i] = mint(i % 2 ? 1 : -1) / mint(i);
        exponential[i] = exponential[i - 1] / mint(i);
    }
    assert(poly_log({1, 1}, n) == logarithm);
    assert(poly_exp({0, 1}, n) == exponential);
}

void test_random(mt19937_64 &rng) {
    for (int iteration = 0; iteration < 220; ++iteration) {
        int n = rng() % 160;
        poly a(rng() % 180), b(1 + rng() % 100);
        for (mint &x : a) x = rng() % MOD;
        for (mint &x : b) x = rng() % MOD;
        b.back() = 1 + rng() % (MOD - 1);
        if (iteration % 3 == 0) {
            a.resize(a.size() + 3);
            b.resize(b.size() + 2);
        }
        auto [quotient, remainder] = poly_divmod(a, b);
        assert(make_pair(quotient, remainder) == slow_divide(a, b));
        poly normalized_a = a;
        poly_trim(normalized_a);
        assert(poly_add(poly_mul(b, quotient), remainder) == normalized_a);
        assert(poly_sub(poly_add(a, b), b) == normalized_a);
        assert(poly_derivative(poly_integral(a)) == normalized_a);

        if (a.empty()) a.push_back(1);
        a[0] = 1 + rng() % (MOD - 1);
        assert(poly_inverse(a, n) == slow_inverse(a, n));
        if (n > 0) {
            assert(slow_multiply(a, poly_inverse(a, n), n) == poly_prefix({1}, n));
        }
        a[0] = 0;
        poly exponential = poly_exp(a, n);
        assert(exponential == slow_exp(a, n));
        assert(poly_log(exponential, n) == poly_prefix(a, n));
        a[0] = 1;
        assert(poly_exp(poly_log(a, n), n) == poly_prefix(a, n));

        poly base(1 + rng() % 40);
        for (mint &x : base) x = rng() % MOD;
        int zeros = rng() % base.size();
        for (int i = 0; i < zeros; ++i) base[i] = 0;
        int64_t exponent = rng() % 8;
        assert(poly_pow(base, exponent, n) == slow_power(base, exponent, n));
    }
    poly base = {5, -3, 2, 1};
    assert(poly_pow(base, numeric_limits<int64_t>::max(), 25) ==
           slow_power(base, numeric_limits<int64_t>::max(), 25));
    // Larger non-power-of-two precisions exercise NTT and Newton truncation.
    poly a(257);
    for (mint &x : a) x = rng() % MOD;
    a[0] = 1;
    assert(poly_inverse(a, 513) == slow_inverse(a, 513));
    a[0] = 0;
    assert(poly_exp(a, 513) == slow_exp(a, 513));
}

signed main() {
    mt19937_64 rng(31274921);
    test_special_cases();
    test_random(rng);
    cout << "polynomial tests passed\n";
}
