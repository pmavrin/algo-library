// Minimal bottom-up sum segment tree.
//
// Unlike segtree_sum.cpp, this version uses exactly 2*n cells and does not
// round n up to a power of two. Use it when constants and memory matter.
// For speed, the caller is responsible for passing valid indices and ranges.

struct fast_segtree {
    int n = 0;
    vector<int64_t> tree;

    explicit fast_segtree(int n) : n(n), tree(2 * n, 0) {}

    explicit fast_segtree(const vector<int64_t> &values)
        : n(static_cast<int>(values.size())), tree(2 * n, 0) {
        for (int i = 0; i < n; ++i) tree[n + i] = values[i];
        for (int i = n - 1; i > 0; --i) tree[i] = tree[2 * i] + tree[2 * i + 1];
    }

    void set(int index, int64_t value) {
        int vertex = index + n;
        tree[vertex] = value;
        for (; vertex > 1; vertex >>= 1) {
            tree[vertex >> 1] = tree[vertex] + tree[vertex ^ 1];
        }
    }

    void add(int index, int64_t delta) {
        int vertex = index + n;
        tree[vertex] += delta;
        for (; vertex > 1; vertex >>= 1) {
            tree[vertex >> 1] = tree[vertex] + tree[vertex ^ 1];
        }
    }

    int64_t get(int index) const {
        return tree[index + n];
    }

    int64_t sum(int left, int right) const {
        int64_t result = 0;
        for (left += n, right += n; left < right; left >>= 1, right >>= 1) {
            if (left & 1) result += tree[left++];
            if (right & 1) result += tree[--right];
        }
        return result;
    }

    int64_t total() const {
        return n == 0 ? 0 : tree[1];
    }
};
