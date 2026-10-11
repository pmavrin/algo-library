// Requires math/mod.cpp (mint and convolution); poly.cpp is not required.
// Recurrence convention, k = recurrence.size():
// s[i] = recurrence[0]*s[i-1] + ... + recurrence[k-1]*s[i-k], i >= k.
// Indices start at zero. Trailing zero recurrence coefficients are meaningful.

// Shortest recurrence fitting the supplied prefix, in O(s.size()^2) time.
// Empty/all-zero prefixes return {}. Works over the prime field used by mint.
// To recover an unknown recurrence of order <= k, provide at least 2*k terms.
// A finite prefix alone does not guarantee its continuation follows a recurrence.
vector<mint> berlekamp_massey(const vector<mint> &sequence) {
    vector<mint> connection{1}, previous{1};
    int length = 0, shift = 1;
    mint previous_discrepancy = 1;
    for (int i = 0; i < static_cast<int>(sequence.size()); ++i) {
        mint discrepancy = sequence[i];
        for (int j = 1; j <= length; ++j) discrepancy += connection[j] * sequence[i - j];
        if (discrepancy == mint(0)) {
            ++shift;
            continue;
        }
        vector<mint> saved = connection;
        mint factor = discrepancy / previous_discrepancy;
        connection.resize(max(connection.size(), previous.size() + size_t(shift)));
        for (size_t j = 0; j < previous.size(); ++j) connection[j + shift] -= factor * previous[j];
        if (2 * length <= i) {
            length = i + 1 - length;
            previous = move(saved);
            previous_discrepancy = discrepancy;
            shift = 1;
        } else {
            ++shift;
        }
    }
    connection.resize(length + 1);
    vector<mint> recurrence(length);
    for (int i = 0; i < length; ++i) recurrence[i] = -connection[i + 1];
    return recurrence;
}

// Bostan-Mori: O(k log k log(n+1)) with NTT, O(k) auxiliary memory.
// initial must contain at least k terms; only the first k are used.
// An empty recurrence defines the all-zero sequence. n must be nonnegative.
mint linear_recurrence_nth(
    const vector<mint> &initial,
    const vector<mint> &recurrence,
    int64_t n
) {
    assert(n >= 0);
    int k = static_cast<int>(recurrence.size());
    assert(initial.size() >= size_t(k));
    if (k == 0) return 0;
    if (n < k) return initial[size_t(n)];
    vector<mint> denominator(k + 1);
    denominator[0] = 1;
    for (int i = 0; i < k; ++i) denominator[i + 1] = -recurrence[i];
    vector<mint> numerator = convolution(vector<mint>(initial.begin(), initial.begin() + k), denominator);
    numerator.resize(k);
    while (n > 0) {
        vector<mint> reflected = denominator;
        for (int i = 1; i <= k; i += 2) reflected[i] = -reflected[i];
        vector<mint> product = convolution(numerator, reflected);
        vector<mint> even_product = convolution(denominator, reflected);
        for (int i = 0; i < k; ++i) numerator[i] = product[2 * i + (n & 1)];
        for (int i = 0; i <= k; ++i) denominator[i] = even_product[2 * i];
        n >>= 1;
    }
    return numerator[0] / denominator[0];
}

// Convenience wrapper: infer a recurrence and extrapolate the nonempty prefix.
// For repeated queries, compute berlekamp_massey once and use linear_recurrence_nth.
mint sequence_nth(const vector<mint> &prefix, int64_t n) {
    assert(!prefix.empty() && n >= 0);
    if (uint64_t(n) < prefix.size()) return prefix[size_t(n)];
    return linear_recurrence_nth(prefix, berlekamp_massey(prefix), n);
}
