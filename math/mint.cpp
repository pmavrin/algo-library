const  int mod = 998244353;

struct mint {
    int value = 0;

    constexpr mint() : value() {}

    mint(const long &x) {
        value = normalize(x);
    }

    static long normalize(const long &x) {
        long v = x % mod;
        if (v < 0) v += mod;
        return v;
    }

    bool operator==(const mint &x) { return value == x.value; };

    mint operator+(const mint &x) { return value + x.value; };

    mint operator-(const mint &x) { return value - x.value; };

    mint operator*(const mint &x) { return (long) value * x.value; };

    void operator+=(const mint &x) { value = normalize(value + x.value); };

    void operator-=(const mint &x) { value = normalize(value - x.value); };
};

mint power(mint a, long b) {
    mint res = 1;
    while (b > 0) {
        if (b & 1) {
            res = res * a;
        }
        a = a * a;
        b >>= 1;
    }
    return res;
}

mint inv(mint a) {
    return power(a, mod - 2);
}

vector<mint> fact_precalc(1, 1);
vector<mint> inv_fact_precalc(1, 1);

void precalc(int n) {
    fact_precalc.resize(n + 1);
    fact_precalc[0] = 1;
    for (int i = 1; i < fact_precalc.size(); i++) {
        fact_precalc[i] = fact_precalc[i - 1] * i;
    }
    inv_fact_precalc.resize(n + 1);
    inv_fact_precalc[n] = inv(fact_precalc[n]);
    for (int i = n - 1; i >= 0; i--) {
        inv_fact_precalc[i] = inv_fact_precalc[i + 1] * (i + 1);
    }
}

void ensure_fact(int n) {
    while (n >= fact_precalc.size()) {
        fact_precalc.push_back(fact_precalc.back() * fact_precalc.size());
        inv_fact_precalc.push_back(inv(fact_precalc.back()));
    }
}

mint fact(int n) {
    ensure_fact(n);
    return fact_precalc[n];
}

mint inv_fact(int n) {
    ensure_fact(n);
    return inv_fact_precalc[n];
}

mint calc_c(int n, int k) {
    if (n < 0 || k < 0 || k > n) {
        return 0;
    }
    mint res = fact(n);
    res = res * inv_fact(k);
    res = res * inv_fact(n - k);
    return res;
}
