#include <bits/stdc++.h>

#ifndef MATH_NO_INT_MACROS
#define int int64_t
#define long int64_t
#endif

#include "../math/mod.cpp"
#include "../math/berlekamp_massey.cpp"

vector<mint> generate_sequence(vector<mint> initial, const vector<mint> &recurrence, int n) {
    int k = static_cast<int>(recurrence.size());
    initial.resize(n);
    for (int i = k; i < n; ++i) {
        initial[i] = 0;
        for (int j = 0; j < k; ++j) initial[i] += recurrence[j] * initial[i - 1 - j];
    }
    return initial;
}

// Independent Gaussian elimination oracle for existence of an order-k recurrence.
bool has_recurrence(const vector<mint> &s, int k) {
    int rows = static_cast<int>(s.size()) - k;
    vector<vector<mint>> matrix(rows, vector<mint>(k + 1));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < k; ++j) matrix[i][j] = s[i + k - 1 - j];
        matrix[i][k] = s[i + k];
    }
    int rank = 0;
    for (int column = 0; column < k && rank < rows; ++column) {
        int pivot = rank;
        while (pivot < rows && matrix[pivot][column] == mint(0)) ++pivot;
        if (pivot == rows) continue;
        swap(matrix[pivot], matrix[rank]);
        mint inverse = matrix[rank][column].inv();
        for (int j = column; j <= k; ++j) matrix[rank][j] *= inverse;
        for (int i = rank + 1; i < rows; ++i) {
            mint factor = matrix[i][column];
            for (int j = column; j <= k; ++j) matrix[i][j] -= factor * matrix[rank][j];
        }
        ++rank;
    }
    for (int i = rank; i < rows; ++i) if (matrix[i][k] != mint(0)) return false;
    return true;
}

using matrix = vector<vector<mint>>;

matrix matrix_multiply(const matrix &a, const matrix &b) {
    size_t n = a.size();
    matrix result(n, vector<mint>(n));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            for (size_t t = 0; t < n; ++t) result[i][j] += a[i][t] * b[t][j];
    return result;
}

mint matrix_nth(const vector<mint> &initial, const vector<mint> &recurrence, int64_t n) {
    int k = static_cast<int>(recurrence.size());
    if (k == 0) return 0;
    if (n < k) return initial[n];
    matrix transition(k, vector<mint>(k)), result(k, vector<mint>(k));
    transition[0] = recurrence;
    for (int i = 1; i < k; ++i) transition[i][i - 1] = 1;
    for (int i = 0; i < k; ++i) result[i][i] = 1;
    n -= k - 1;
    while (n > 0) {
        if (n & 1) result = matrix_multiply(result, transition);
        transition = matrix_multiply(transition, transition);
        n >>= 1;
    }
    mint answer = 0;
    for (int i = 0; i < k; ++i) answer += result[0][i] * initial[k - 1 - i];
    return answer;
}

void test_known() {
    assert(berlekamp_massey({}).empty());
    assert(berlekamp_massey({0, 0, 0}).empty());
    assert(berlekamp_massey({0, 1, 1, 2, 3, 5}) == vector<mint>({1, 1}));
    assert(berlekamp_massey({3, 3, 3, 3}) == vector<mint>{mint(1)});
    assert(berlekamp_massey({1, 0, 0, 0}) == vector<mint>{mint(0)});
    assert(berlekamp_massey({0, 0, 1, 0, 0, 0}) == vector<mint>({0, 0, 0}));
    assert(linear_recurrence_nth({}, {}, 100) == mint(0));
    assert(linear_recurrence_nth({0, 1}, {1, 1}, 10) == mint(55));
    assert(sequence_nth({0, 1, 1, 2, 3, 5}, 10) == mint(55));
    assert(sequence_nth({0}, 999999) == mint(0));
    assert(sequence_nth({7}, 0) == mint(7));
    const int64_t huge = numeric_limits<int64_t>::max();
    assert(linear_recurrence_nth({7}, {3}, huge) == mint(7) * mint(3).pow(huge));
    assert(linear_recurrence_nth({7}, {0}, huge) == mint(0));
    assert(linear_recurrence_nth({2, 5, 9}, {0, 0, 0}, huge) == mint(0));
}

void test_minimality(mt19937_64 &rng) {
    for (int n = 0; n <= 18; ++n) {
        for (int iteration = 0; iteration < 30; ++iteration) {
            vector<mint> sequence(n);
            for (mint &x : sequence) x = rng() % 4;
            auto recurrence = berlekamp_massey(sequence);
            int k = static_cast<int>(recurrence.size());
            assert(has_recurrence(sequence, k));
            if (k > 0) assert(!has_recurrence(sequence, k - 1));
            for (int i = k; i < n; ++i) {
                mint expected = 0;
                for (int j = 0; j < k; ++j) expected += recurrence[j] * sequence[i - j - 1];
                assert(sequence[i] == expected);
            }
        }
    }
}

void test_random(mt19937_64 &rng) {
    for (int iteration = 0; iteration < 160; ++iteration) {
        int k = 1 + rng() % 12;
        vector<mint> initial(k), recurrence(k);
        for (mint &x : initial) x = rng() % MOD;
        for (mint &x : recurrence) x = rng() % MOD;
        if (iteration % 3 == 0) recurrence.back() = 0;
        vector<mint> sequence = generate_sequence(initial, recurrence, 150);
        vector<mint> prefix(sequence.begin(), sequence.begin() + 2 * k);
        auto inferred = berlekamp_massey(prefix);
        assert(inferred.size() <= size_t(k));
        for (int probe = 0; probe < 12; ++probe) {
            int n = rng() % sequence.size();
            assert(linear_recurrence_nth(initial, recurrence, n) == sequence[n]);
            assert(linear_recurrence_nth(prefix, inferred, n) == sequence[n]);
            assert(sequence_nth(prefix, n) == sequence[n]);
        }
        if (k <= 5) {
            int64_t n = numeric_limits<int64_t>::max() - int64_t(rng() % 1000);
            assert(linear_recurrence_nth(initial, recurrence, n) == matrix_nth(initial, recurrence, n));
        }
    }
    // Nonminimal and larger recurrences, including the NTT path.
    for (int k : {33, 64, 127}) {
        vector<mint> initial(k), recurrence(k);
        for (mint &x : initial) x = rng() % MOD;
        for (mint &x : recurrence) x = rng() % MOD;
        auto sequence = generate_sequence(initial, recurrence, 1000);
        auto inferred = berlekamp_massey(vector<mint>(sequence.begin(), sequence.begin() + 2 * k));
        assert(linear_recurrence_nth(sequence, inferred, 999) == sequence[999]);
        assert(linear_recurrence_nth(initial, recurrence, 997) == sequence[997]);
    }
}

signed main() {
    mt19937_64 rng(7123667);
    test_known();
    test_minimality(rng);
    test_random(rng);
    cout << "recurrence tests passed\n";
}
