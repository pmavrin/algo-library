#include <bits/stdc++.h>
using namespace std;

#ifndef CHT_NO_INT_MACROS
#define int int64_t
#define long int64_t
#endif

#include "../data_structures/cht.cpp"
#include "../data_structures/li_chao.cpp"

using test_line = pair<int64_t, int64_t>;

__int128_t brute(const vector<test_line> &lines, int64_t x) {
    assert(!lines.empty());
    __int128_t result = __int128_t(lines[0].first) * x + lines[0].second;
    for (auto [k, b] : lines) result = min(result, __int128_t(k) * x + b);
    return result;
}

void test_boundaries() {
    const int64_t lo = numeric_limits<int64_t>::min();
    const int64_t hi = numeric_limits<int64_t>::max();
    vector<int64_t> values = {lo, lo + 1, -1, 0, 1, hi - 1, hi};
    li_chao tree(lo, hi);
    cht hull;
    vector<test_line> lines;
    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        for (int64_t b : values) {
            lines.emplace_back(*it, b);
            hull.add_line(*it, b);
            tree.add_line(*it, b);
            for (int64_t x : values) {
                assert(hull.query(x) == brute(lines, x));
                assert(tree.query(x) == brute(lines, x));
            }
        }
    }
    for (int64_t x : values) assert(hull.query_monotone(x) == brute(lines, x));
    hull.clear();
    tree.clear();
    assert(hull.empty() && tree.empty());
    hull.add_line(1, 2);
    tree.add_line(1, 2);
    assert(hull.query_monotone(-10) == -8);
    assert(tree.query(-10) == -8);

    // Fractional switch points of either sign; equality at integer switches.
    for (int64_t b : {-7, -6, -5, -1, 0, 1, 5, 6, 7}) {
        cht h;
        h.add_line(3, 0);
        h.add_line(0, b);
        for (int64_t x = -10; x <= 10; ++x) {
            assert(h.query(x) == min(__int128_t(3) * x, __int128_t(b)));
        }
    }
    // A switch beyond int64, then one below int64.
    for (int direction : {-1, 1}) {
        cht h;
        h.add_line(1, direction == 1 ? lo : hi);
        h.add_line(0, direction == 1 ? hi : lo);
        for (int64_t x : values) {
            assert(h.query(x) == min(__int128_t(x) + (direction == 1 ? lo : hi),
                                    __int128_t(direction == 1 ? hi : lo)));
        }
    }
    // Point domains, including both extreme coordinates.
    for (int64_t x : values) {
        li_chao single(x, x);
        single.add_line(hi, lo);
        single.add_line(lo, hi);
        assert(single.query(x) == min(__int128_t(hi) * x + lo, __int128_t(lo) * x + hi));
    }
}

void test_random(mt19937_64 &rng) {
    for (int iteration = 0; iteration < 400; ++iteration) {
        int64_t left = -int64_t(rng() % 1000);
        int64_t right = rng() % 1000;
        li_chao tree(left, right);
        cht hull;
        vector<test_line> arbitrary, ordered;
        int64_t slope = 100;
        int64_t monotone_x = -100;
        for (int step = 0; step < 100; ++step) {
            int64_t k = int64_t(rng() % 201) - 100;
            int64_t b = int64_t(rng() % 20001) - 10000;
            tree.add_line(k, b);
            arbitrary.emplace_back(k, b);
            slope -= rng() % 4;
            hull.add_line(slope, b);
            ordered.emplace_back(slope, b);
            // Insertions and monotone/arbitrary queries are interleaved.
            monotone_x += rng() % 4;
            assert(hull.query_monotone(monotone_x) == brute(ordered, monotone_x));
            for (int probe = 0; probe < 8; ++probe) {
                int64_t x = left + int64_t(rng() % (right - left + 1));
                assert(tree.query(x) == brute(arbitrary, x));
                assert(hull.query(x) == brute(ordered, x));
            }
            assert(tree.query(left) == brute(arbitrary, left));
            assert(tree.query(right) == brute(arbitrary, right));
        }
    }
    // Ensure many Li Chao nodes and vector reallocations: tangent lines
    // k = -2*t, b = t^2 have distinct regions where they are optimal.
    vector<int64_t> order(4001);
    iota(order.begin(), order.end(), int64_t(-2000));
    shuffle(order.begin(), order.end(), rng);
    li_chao tree(-2000, 2000);
    for (int64_t t : order) tree.add_line(-2 * t, t * t);
    for (int64_t x = -2000; x <= 2000; ++x) assert(tree.query(x) == -x * x);
}

void test_dp(mt19937_64 &rng) {
    // dp[i] = min_{j<i}(dp[j] + (h[i] - h[j])^2 + penalty).
    // Li Chao works for any h; sorted h also allows monotone CHT.
    for (int iteration = 0; iteration < 300; ++iteration) {
        const int n = 80;
        vector<int64_t> h(n);
        for (int64_t &x : h) x = int64_t(rng() % 401) - 200;
        bool sorted = iteration % 2 == 0;
        if (sorted) sort(h.begin(), h.end());
        int64_t penalty = int64_t(rng() % 201) - 100;
        vector<int64_t> expected(n), actual(n), monotone(n);
        li_chao tree(-200, 200);
        cht hull;
        tree.add_line(-2 * h[0], h[0] * h[0]);
        if (sorted) hull.add_line(-2 * h[0], h[0] * h[0]);
        for (int i = 1; i < n; ++i) {
            expected[i] = numeric_limits<int64_t>::max();
            for (int j = 0; j < i; ++j) {
                int64_t delta = h[i] - h[j];
                expected[i] = min(expected[i], expected[j] + delta * delta + penalty);
            }
            actual[i] = int64_t(tree.query(h[i]) + h[i] * h[i] + penalty);
            assert(actual[i] == expected[i]);
            tree.add_line(-2 * h[i], actual[i] + h[i] * h[i]);
            if (sorted) {
                monotone[i] = int64_t(hull.query_monotone(h[i]) + h[i] * h[i] + penalty);
                assert(monotone[i] == expected[i]);
                hull.add_line(-2 * h[i], monotone[i] + h[i] * h[i]);
            }
        }
    }
}

signed main() {
    mt19937_64 rng(7192381);
    test_boundaries();
    test_random(rng);
    test_dp(rng);
    cout << "CHT and Li Chao tests passed\n";
}
