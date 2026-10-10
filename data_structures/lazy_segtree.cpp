// Range addition and range sum queries on [l, r).
// To adapt the tree, edit only item, modifier, neutral values and the three
// functions merge, apply and compose at the beginning of the struct.

struct lazy_segtree {
    using item = int64_t;
    using modifier = int64_t;

    static item neutral_item() {
        return 0;
    }

    static modifier neutral_modifier() {
        return 0;
    }

    static item merge(item left, item right) {
        return left + right;
    }

    static item apply(item value, modifier change, int length) {
        return value + change * length;
    }

    // Returns a modifier equivalent to applying old_change, then new_change.
    static modifier compose(modifier old_change, modifier new_change) {
        return old_change + new_change;
    }

    int n = 0;
    int size = 1;
    vector<item> tree;
    vector<modifier> lazy;
    vector<char> has_lazy;

    explicit lazy_segtree(int n) : n(n) {
        assert(n >= 0);
        init_storage();
    }

    explicit lazy_segtree(const vector<item> &values)
        : n(static_cast<int>(values.size())) {
        init_storage();
        for (int i = 0; i < n; ++i) tree[size + i] = values[i];
        build();
    }

    void set(int index, item value) {
        assert(0 <= index && index < n);
        set_impl(index, value, 1, 0, size);
    }

    void range_apply(int left, int right, modifier change) {
        assert(0 <= left && left <= right && right <= n);
        range_apply_impl(left, right, change, 1, 0, size);
    }

    item query(int left, int right) {
        assert(0 <= left && left <= right && right <= n);
        return query_impl(left, right, 1, 0, size);
    }

    item get(int index) {
        return query(index, index + 1);
    }

    item all() const {
        return tree[1];
    }

private:
    void init_storage() {
        while (size < n) size <<= 1;
        tree.assign(2 * size, neutral_item());
        lazy.assign(2 * size, neutral_modifier());
        has_lazy.assign(2 * size, false);
    }

    void build() {
        for (int vertex = size - 1; vertex > 0; --vertex) pull(vertex);
    }

    void pull(int vertex) {
        tree[vertex] = merge(tree[2 * vertex], tree[2 * vertex + 1]);
    }

    void apply_to_vertex(int vertex, modifier change, int length) {
        tree[vertex] = apply(tree[vertex], change, length);
        if (has_lazy[vertex]) {
            lazy[vertex] = compose(lazy[vertex], change);
        } else {
            lazy[vertex] = change;
            has_lazy[vertex] = true;
        }
    }

    void push(int vertex, int left, int right) {
        if (!has_lazy[vertex] || right - left == 1) return;
        int middle = (left + right) >> 1;
        apply_to_vertex(2 * vertex, lazy[vertex], middle - left);
        apply_to_vertex(2 * vertex + 1, lazy[vertex], right - middle);
        lazy[vertex] = neutral_modifier();
        has_lazy[vertex] = false;
    }

    void set_impl(int index, item value, int vertex, int left, int right) {
        if (right - left == 1) {
            tree[vertex] = value;
            lazy[vertex] = neutral_modifier();
            has_lazy[vertex] = false;
            return;
        }
        push(vertex, left, right);
        int middle = (left + right) >> 1;
        if (index < middle) set_impl(index, value, 2 * vertex, left, middle);
        else set_impl(index, value, 2 * vertex + 1, middle, right);
        pull(vertex);
    }

    void range_apply_impl(
        int query_left,
        int query_right,
        modifier change,
        int vertex,
        int left,
        int right
    ) {
        if (query_right <= left || right <= query_left) return;
        if (query_left <= left && right <= query_right) {
            apply_to_vertex(vertex, change, right - left);
            return;
        }
        push(vertex, left, right);
        int middle = (left + right) >> 1;
        range_apply_impl(query_left, query_right, change, 2 * vertex, left, middle);
        range_apply_impl(query_left, query_right, change, 2 * vertex + 1, middle, right);
        pull(vertex);
    }

    item query_impl(
        int query_left,
        int query_right,
        int vertex,
        int left,
        int right
    ) {
        if (query_right <= left || right <= query_left) return neutral_item();
        if (query_left <= left && right <= query_right) return tree[vertex];
        push(vertex, left, right);
        int middle = (left + right) >> 1;
        return merge(
            query_impl(query_left, query_right, 2 * vertex, left, middle),
            query_impl(query_left, query_right, 2 * vertex + 1, middle, right)
        );
    }
};
