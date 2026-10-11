// Requires math/mod.cpp. Coefficients are stored in increasing degree order.
// Ordinary polynomial operations trim trailing zeros; the zero polynomial is {}.
// Series operations with a precision n return exactly n coefficients (mod x^n).
// Multiplication uses convolution; its NTT size limit still applies.

using poly = vector<mint>;

void poly_trim(poly &a) {
    while (!a.empty() && a.back() == mint(0)) a.pop_back();
}

poly poly_prefix(const poly &a, int n) {
    assert(n >= 0);
    poly result(a.begin(), a.begin() + min(a.size(), size_t(n)));
    result.resize(n);
    return result;
}

poly poly_add(poly a, const poly &b) {
    a.resize(max(a.size(), b.size()));
    for (size_t i = 0; i < b.size(); ++i) a[i] += b[i];
    poly_trim(a);
    return a;
}

poly poly_sub(poly a, const poly &b) {
    a.resize(max(a.size(), b.size()));
    for (size_t i = 0; i < b.size(); ++i) a[i] -= b[i];
    poly_trim(a);
    return a;
}

poly poly_mul(const poly &a, const poly &b) {
    poly result = convolution(a, b);
    poly_trim(result);
    return result;
}

mint poly_evaluate(const poly &a, mint x) {
    mint result = 0;
    for (auto it = a.rbegin(); it != a.rend(); ++it) result = result * x + *it;
    return result;
}

poly poly_derivative(const poly &a) {
    if (a.empty()) return {};
    poly result(a.size() - 1);
    for (size_t i = 1; i < a.size(); ++i) result[i - 1] = a[i] * mint(i);
    poly_trim(result);
    return result;
}

// Constant of integration is zero. Requires a.size() < MOD.
poly poly_integral(const poly &a) {
    assert(a.size() < size_t(MOD));
    if (a.empty()) return {};
    int n = static_cast<int>(a.size());
    vector<mint> inverses(n + 1);
    inverses[1] = 1;
    for (int i = 2; i <= n; ++i) {
        inverses[i] = -mint(MOD / i) * inverses[MOD % i];
    }
    poly result(n + 1);
    for (int i = 1; i <= n; ++i) result[i] = a[i - 1] * inverses[i];
    poly_trim(result);
    return result;
}

// 1/a mod x^n. Requires a[0] != 0 when n > 0. O(n log n).
poly poly_inverse(const poly &a, int n) {
    assert(n >= 0);
    if (n == 0) return {};
    assert(!a.empty() && a[0] != mint(0));
    poly result = {a[0].inv()};
    while (static_cast<int>(result.size()) < n) {
        int next_size = min(n, 2 * static_cast<int>(result.size()));
        poly correction = convolution(poly_prefix(a, next_size), result);
        correction.resize(next_size);
        for (mint &value : correction) value = -value;
        correction[0] += 2;
        result = convolution(result, correction);
        result.resize(next_size);
    }
    return result;
}

// a = b*q + r, deg(r) < deg(b). The divisor must be nonzero.
// O(n log n), n = max(a.size(), b.size()).
pair<poly, poly> poly_divmod(poly a, poly b) {
    poly_trim(a);
    poly_trim(b);
    assert(!b.empty());
    if (a.size() < b.size()) return {{}, a};
    int size = static_cast<int>(a.size() - b.size() + 1);
    poly reversed_a(a.rbegin(), a.rend());
    reversed_a.resize(size);
    poly reversed_b(b.rbegin(), b.rend());
    poly quotient = convolution(reversed_a, poly_inverse(reversed_b, size));
    quotient.resize(size);
    reverse(quotient.begin(), quotient.end());
    poly remainder = poly_sub(a, poly_mul(b, quotient));
    poly_trim(quotient);
    return {quotient, remainder};
}

// log(a) mod x^n. Requires a[0] == 1, 0 <= n < MOD. O(n log n).
poly poly_log(const poly &a, int n) {
    assert(0 <= n && n < MOD);
    if (n == 0) return {};
    assert(!a.empty() && a[0] == mint(1));
    poly result = convolution(poly_derivative(poly_prefix(a, n)), poly_inverse(a, n));
    result.resize(n - 1);
    result = poly_integral(result);
    result.resize(n);
    return result;
}

// exp(a) mod x^n. Requires a[0] == 0 (or a empty), 0 <= n < MOD.
// O(n log n), using Newton iteration.
poly poly_exp(const poly &a, int n) {
    assert(0 <= n && n < MOD);
    if (n == 0) return {};
    assert(a.empty() || a[0] == mint(0));
    poly result = {1};
    while (static_cast<int>(result.size()) < n) {
        int next_size = min(n, 2 * static_cast<int>(result.size()));
        poly logarithm = poly_log(result, next_size);
        poly correction = poly_prefix(a, next_size);
        for (int i = 0; i < next_size; ++i) correction[i] -= logarithm[i];
        correction[0] += 1;
        result = convolution(result, correction);
        result.resize(next_size);
    }
    return result;
}

// a^exponent mod x^n, exponent >= 0, 0 <= n < MOD. Defines 0^0 = 1.
// Handles leading zeros and arbitrary constant terms. O(n log n + log(exponent)).
poly poly_pow(const poly &a, int64_t exponent, int n) {
    assert(exponent >= 0 && 0 <= n && n < MOD);
    if (n == 0) return {};
    poly result(n);
    if (exponent == 0) {
        result[0] = 1;
        return result;
    }
    int first = 0;
    while (first < n && first < static_cast<int>(a.size()) && a[first] == mint(0)) ++first;
    if (first == n || first == static_cast<int>(a.size())) return result;
    // Check before multiplying to avoid overflowing first * exponent.
    if (first > 0 && exponent > (n - 1) / first) return result;
    int shift = static_cast<int>(first * exponent);
    int size = n - shift;
    mint constant = a[first], inverse_constant = constant.inv();
    poly unit(size);
    for (int i = 0; i < size && first + i < static_cast<int>(a.size()); ++i) {
        unit[i] = a[first + i] * inverse_constant;
    }
    poly logarithm = poly_log(unit, size);
    for (mint &value : logarithm) value *= mint(exponent);
    poly powered = poly_exp(logarithm, size);
    mint scale = constant.pow(exponent);
    for (int i = 0; i < size; ++i) result[shift + i] = powered[i] * scale;
    return result;
}
