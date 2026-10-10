// Ready-to-use tree for sums, point updates and prefix-sum search.
// lower_bound_prefix requires all values to be non-negative.

struct sum_segtree {
    int n = 0;
    int size = 1;
    vector<int64_t> tree;

    explicit sum_segtree(int n) : n(n) {
        assert(n >= 0);
        init_storage();
    }

    explicit sum_segtree(const vector<int64_t> &values)
        : n(static_cast<int>(values.size())) {
        init_storage();
        for (int i = 0; i < n; ++i) tree[size + i] = values[i];
        build();
    }

    void set(int index, int64_t value) {
        assert(0 <= index && index < n);
        int vertex = size + index;
        tree[vertex] = value;
        while (vertex > 1) {
            vertex >>= 1;
            tree[vertex] = tree[2 * vertex] + tree[2 * vertex + 1];
        }
    }

    void add(int index, int64_t delta) {
        assert(0 <= index && index < n);
        set(index, tree[size + index] + delta);
    }

    int64_t get(int index) const {
        assert(0 <= index && index < n);
        return tree[size + index];
    }

    int64_t sum(int left, int right) const {
        assert(0 <= left && left <= right && right <= n);
        int64_t result = 0;
        for (left += size, right += size; left < right; left >>= 1, right >>= 1) {
            if (left & 1) result += tree[left++];
            if (right & 1) result += tree[--right];
        }
        return result;
    }

    int64_t total() const {
        return tree[1];
    }

    // Returns the smallest i such that sum(0, i + 1) >= target.
    // Returns n if the total sum is smaller than target.
    int lower_bound_prefix(int64_t target) const {
        if (target <= 0) return 0;
        if (tree[1] < target) return n;

        int vertex = 1;
        while (vertex < size) {
            if (tree[2 * vertex] >= target) {
                vertex = 2 * vertex;
            } else {
                target -= tree[2 * vertex];
                vertex = 2 * vertex + 1;
            }
        }
        return min(vertex - size, n);
    }

    // Treats values as multiplicities and finds the zero-based k-th element.
    int find_by_order(int64_t k) const {
        if (k < 0 || k >= total()) return n;
        return lower_bound_prefix(k + 1);
    }

private:
    void init_storage() {
        while (size < n) size <<= 1;
        tree.assign(2 * size, 0);
    }

    void build() {
        for (int vertex = size - 1; vertex > 0; --vertex) {
            tree[vertex] = tree[2 * vertex] + tree[2 * vertex + 1];
        }
    }
};
