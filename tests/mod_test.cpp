#include <bits/stdc++.h>

#define int int64_t
#define long int64_t
#include "../math/mod.cpp"

signed main() {
    assert(mint(-1).value == MOD - 1);
    assert((mint(10) / 2).value == 5);
    assert(power(2, 10).value == 1024);
    assert(C(5, 2).value == 10);
    assert(C(5, -1).value == 0);
    assert(P(5, 2).value == 20);
    assert(comb.multinomial({2, 1, 2}).value == 30);

    mt19937 rng(712367);
    for (int n = 1; n <= 128; n <<= 1) {
        vector<mint> a(n);
        for (mint &x : a) x = rng() % MOD;
        vector<mint> original = a;
        ntt(a);
        ntt(a, true);
        assert(a == original);
    }

    for (int iteration = 0; iteration < 200; ++iteration) {
        int n = 1 + rng() % 100;
        int m = 1 + rng() % 100;
        vector<mint> a(n), b(m), expected(n + m - 1);
        for (mint &x : a) x = rng() % MOD;
        for (mint &x : b) x = rng() % MOD;
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j)
                expected[i + j] += a[i] * b[j];
        assert(convolution(a, b) == expected);
    }
}
