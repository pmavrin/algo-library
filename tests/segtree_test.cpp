#include <bits/stdc++.h>

#define int int64_t
#define long int64_t

#include "../data_structures/segtree.cpp"
#include "../data_structures/lazy_segtree.cpp"
#include "../data_structures/segtree_sum.cpp"
#include "../data_structures/segtree_fast.cpp"
#include "../data_structures/segtree_two_max_gcd.cpp"

int64_t slow_sum(const vector<int64_t> &a, int left, int right) {
    return accumulate(a.begin() + left, a.begin() + right, int64_t(0));
}

void test_generic_segtree(mt19937 &rng) {
    for (int n = 0; n <= 70; ++n) {
        vector<int64_t> a(n);
        for (int64_t &x : a) x = int(rng() % 201) - 100;
        segtree tree(a);

        for (int iteration = 0; iteration < 500; ++iteration) {
            if (n > 0 && rng() % 2 == 0) {
                int index = rng() % n;
                int64_t value = int(rng() % 201) - 100;
                a[index] = value;
                tree.set(index, value);
                assert(tree.get(index) == value);
            } else {
                int left = n == 0 ? 0 : rng() % (n + 1);
                int right = left + (n == left ? 0 : rng() % (n - left + 1));
                assert(tree.query(left, right) == slow_sum(a, left, right));
            }
            assert(tree.all() == slow_sum(a, 0, n));
        }
    }

}

void test_lazy_segtree(mt19937 &rng) {
    for (int n = 0; n <= 70; ++n) {
        vector<int64_t> a(n);
        for (int64_t &x : a) x = int(rng() % 201) - 100;
        lazy_segtree tree(a);

        for (int iteration = 0; iteration < 700; ++iteration) {
            int operation = rng() % 3;
            if (n > 0 && operation == 0) {
                int left = rng() % (n + 1);
                int right = left + (n == left ? 0 : rng() % (n - left + 1));
                int64_t delta = int(rng() % 41) - 20;
                for (int i = left; i < right; ++i) a[i] += delta;
                tree.range_apply(left, right, delta);
            } else if (n > 0 && operation == 1) {
                int index = rng() % n;
                int64_t value = int(rng() % 201) - 100;
                a[index] = value;
                tree.set(index, value);
                assert(tree.get(index) == value);
            } else {
                int left = n == 0 ? 0 : rng() % (n + 1);
                int right = left + (n == left ? 0 : rng() % (n - left + 1));
                assert(tree.query(left, right) == slow_sum(a, left, right));
            }
            assert(tree.all() == slow_sum(a, 0, n));
        }
    }
}

void test_sum_trees(mt19937 &rng) {
    for (int n = 0; n <= 100; ++n) {
        vector<int64_t> a(n);
        for (int64_t &x : a) x = rng() % 20;
        sum_segtree regular(a);
        fast_segtree fast(a);

        for (int iteration = 0; iteration < 700; ++iteration) {
            int operation = rng() % 3;
            if (n > 0 && operation == 0) {
                int index = rng() % n;
                int64_t value = rng() % 20;
                a[index] = value;
                regular.set(index, value);
                fast.set(index, value);
            } else if (n > 0 && operation == 1) {
                int index = rng() % n;
                int64_t delta = rng() % 10;
                a[index] += delta;
                regular.add(index, delta);
                fast.add(index, delta);
            } else {
                int left = n == 0 ? 0 : rng() % (n + 1);
                int right = left + (n == left ? 0 : rng() % (n - left + 1));
                int64_t expected = slow_sum(a, left, right);
                assert(regular.sum(left, right) == expected);
                assert(fast.sum(left, right) == expected);
            }

            assert(regular.total() == slow_sum(a, 0, n));
            assert(fast.total() == regular.total());
            int64_t target = 1 + rng() % (regular.total() + 1);
            int expected = n;
            int64_t prefix = 0;
            for (int i = 0; i < n; ++i) {
                prefix += a[i];
                if (prefix >= target) {
                    expected = i;
                    break;
                }
            }
            assert(regular.lower_bound_prefix(target) == expected);
        }
    }
}

void test_specialized_tree() {
    using item = two_max_gcd_segtree::item;
    vector<item> values = {{2, 0, 0}, {4, 0, 0}, {6, 0, 0}, {8, 0, 0}};
    two_max_gcd_segtree tree(values);
    item result = tree.query(0, 4);
    assert(result.max1 == 8);
    assert(result.max2 == 6);
    assert(result.rest == 2);
}

signed main() {
    mt19937 rng(712367);
    test_generic_segtree(rng);
    test_lazy_segtree(rng);
    test_sum_trees(rng);
    test_specialized_tree();
}
