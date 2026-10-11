// Minimum of k*x + b on the inclusive integer domain [min_x, max_x].
// Slopes and queries may arrive in any order; no coordinate compression.
// add_line/query: O(log(domain length)), at most 65 visited nodes per operation.
// O(number of inserted lines) memory. Full int64_t domains are supported.
// Coefficients and x are int64_t; answers use __int128_t.

struct li_chao {
    explicit li_chao(int64_t min_x, int64_t max_x)
        : domain_left(min_x), domain_right(max_x) {
        assert(min_x <= max_x);
    }

    bool empty() const { return root == -1; }

    void clear() {
        nodes.clear();
        root = -1;
    }

    void add_line(int64_t k, int64_t b) {
        root = insert(root, domain_left, domain_right, {k, b});
    }

    __int128_t query(int64_t x) const {
        assert(!empty());
        assert(domain_left <= x && x <= domain_right);
        int vertex = root;
        int64_t left = domain_left, right = domain_right;
        __int128_t result = nodes[vertex].value.eval(x);
        while (vertex != -1) {
            result = min(result, nodes[vertex].value.eval(x));
            if (left == right) break;
            int64_t middle = midpoint(left, right);
            if (x <= middle) {
                vertex = nodes[vertex].left;
                right = middle;
            } else {
                vertex = nodes[vertex].right;
                left = middle + 1;
            }
        }
        return result;
    }

private:
    struct line {
        int64_t k, b;

        __int128_t eval(int64_t x) const {
            return __int128_t(k) * x + b;
        }
    };

    struct node {
        line value;
        int left = -1, right = -1;
    };

    int64_t domain_left, domain_right;
    vector<node> nodes;
    int root = -1;

    static int64_t midpoint(int64_t left, int64_t right) {
        return int64_t(__int128_t(left) + (__int128_t(right) - left) / 2);
    }

    int insert(int vertex, int64_t left, int64_t right, line candidate) {
        if (vertex == -1) {
            nodes.push_back({candidate, -1, -1});
            return static_cast<int>(nodes.size()) - 1;
        }
        int64_t middle = midpoint(left, right);
        if (candidate.eval(middle) < nodes[vertex].value.eval(middle)) {
            swap(candidate, nodes[vertex].value);
        }
        if (left == right) return vertex;
        // The losing line can improve the answer on at most one side.
        // Store indices, not references: recursion may reallocate nodes.
        if (candidate.eval(left) < nodes[vertex].value.eval(left)) {
            int child = insert(nodes[vertex].left, left, middle, candidate);
            nodes[vertex].left = child;
        } else if (candidate.eval(right) < nodes[vertex].value.eval(right)) {
            int child = insert(nodes[vertex].right, middle + 1, right, candidate);
            nodes[vertex].right = child;
        }
        return vertex;
    }
};
