// Point assignment and range queries on [l, r).
// By default this is a sum tree. To use another operation, edit only the
// item, neutral_item and merge definitions at the beginning of the struct.

struct segtree {
    using item = int64_t;

    static item neutral_item() {
        return 0;
    }

    static item merge(item left, item right) {
        return left + right;
    }

    int n = 0;
    int size = 1;
    vector<item> tree;

    explicit segtree(int n) : n(n) {
        assert(n >= 0);
        init_storage();
    }

    explicit segtree(const vector<item> &values)
        : n(static_cast<int>(values.size())) {
        init_storage();
        for (int i = 0; i < n; ++i) tree[size + i] = values[i];
        build();
    }

    void set(int index, item value) {
        assert(0 <= index && index < n);
        int vertex = size + index;
        tree[vertex] = value;
        while (vertex > 1) {
            vertex >>= 1;
            tree[vertex] = merge(tree[2 * vertex], tree[2 * vertex + 1]);
        }
    }

    item get(int index) const {
        assert(0 <= index && index < n);
        return tree[size + index];
    }

    item query(int left, int right) const {
        assert(0 <= left && left <= right && right <= n);
        item left_result = neutral_item();
        item right_result = neutral_item();
        for (left += size, right += size; left < right; left >>= 1, right >>= 1) {
            if (left & 1) left_result = merge(left_result, tree[left++]);
            if (right & 1) right_result = merge(tree[--right], right_result);
        }
        return merge(left_result, right_result);
    }

    item all() const {
        return tree[1];
    }

private:
    void init_storage() {
        while (size < n) size <<= 1;
        tree.assign(2 * size, neutral_item());
    }

    void build() {
        for (int vertex = size - 1; vertex > 0; --vertex) {
            tree[vertex] = merge(tree[2 * vertex], tree[2 * vertex + 1]);
        }
    }
};
