pair<long, long> ext_euclidean(long a, long b) {
    if (b == 0) return {1, 0};
    long k = a / b;
    auto [x, y] = ext_euclidean(b, a - k * b);
    return {y, x - k * y};
}

pair<long, long> crt(long n, long a, long m, long b) {
    long g = gcd(n, m);
    long d = a - b;
    if (d % g != 0) return {-1, -1};
    auto [x, y] = ext_euclidean(n, m);
    long nn = n * (m / g);
    __int128_t aa = (a - (__int128_t)x * (d / g) % nn * n) % nn;
    if (aa < 0) aa += nn;
    return {nn, aa};
}
