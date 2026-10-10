// Specialized tree kept from the old segtree.cpp.
// A node stores the two largest distinct non-negative values and the gcd of
// all remaining distinct values. Point assignment, range query on [l, r).

struct two_max_gcd_segtree {
    struct item {
        int max1 = 0;
        int max2 = 0;
        int rest = 0;
    };

    int n = 0;
    int size = 1;
    vector<item> tree;

    explicit two_max_gcd_segtree(int n) : n(n) {
        assert(n >= 0);
        init_storage();
    }

    explicit two_max_gcd_segtree(const vector<item> &values)
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

    item query(int left, int right) const {
        assert(0 <= left && left <= right && right <= n);
        item left_result;
        item right_result;
        for (left += size, right += size; left < right; left >>= 1, right >>= 1) {
            if (left & 1) left_result = merge(left_result, tree[left++]);
            if (right & 1) right_result = merge(tree[--right], right_result);
        }
        return merge(left_result, right_result);
    }

private:
    static item merge(item left, item right) {
        item result;
        result.rest = gcd(left.rest, right.rest);
        for (int value : {left.max1, left.max2, right.max1, right.max2}) {
            if (value == result.max1 || value == result.max2) continue;
            if (value > result.max1) {
                result.rest = gcd(result.rest, result.max2);
                result.max2 = result.max1;
                result.max1 = value;
            } else if (value > result.max2) {
                result.rest = gcd(result.rest, result.max2);
                result.max2 = value;
            } else {
                result.rest = gcd(result.rest, value);
            }
        }
        return result;
    }

    void init_storage() {
        while (size < n) size <<= 1;
        tree.assign(2 * size, item());
    }

    void build() {
        for (int vertex = size - 1; vertex > 0; --vertex) {
            tree[vertex] = merge(tree[2 * vertex], tree[2 * vertex + 1]);
        }
    }
};
