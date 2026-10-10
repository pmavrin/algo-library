// Binary lifting for a tree or forest. Vertices are numbered [0, n).

struct lca_binary_lifting {
    int n = 0;
    int levels = 1;
    vector<vector<int>> up;
    vector<int> depth;
    vector<int> component;

    explicit lca_binary_lifting(const vector<vector<int>> &tree, int root = 0)
        : n(static_cast<int>(tree.size())) {
        while ((int64_t(1) << levels) <= max<int>(1, n)) ++levels;
        up.assign(levels, vector<int>(n, -1));
        depth.assign(n, 0);
        component.assign(n, -1);
        if (n == 0) return;
        assert(0 <= root && root < n);

        build_component(tree, root, 0);
        int components = 1;
        for (int vertex = 0; vertex < n; ++vertex) {
            if (component[vertex] == -1) build_component(tree, vertex, components++);
        }

        for (int level = 1; level < levels; ++level) {
            for (int vertex = 0; vertex < n; ++vertex) {
                up[level][vertex] = up[level - 1][up[level - 1][vertex]];
            }
        }
    }

    int kth_ancestor(int vertex, int distance) const {
        assert(0 <= vertex && vertex < n && distance >= 0);
        if (distance > depth[vertex]) return -1;
        for (int level = 0; level < levels; ++level) {
            if (distance & (int64_t(1) << level)) vertex = up[level][vertex];
        }
        return vertex;
    }

    int lca(int first, int second) const {
        assert(0 <= first && first < n && 0 <= second && second < n);
        if (component[first] != component[second]) return -1;
        if (depth[first] < depth[second]) swap(first, second);
        first = kth_ancestor(first, depth[first] - depth[second]);
        if (first == second) return first;
        for (int level = levels - 1; level >= 0; --level) {
            if (up[level][first] != up[level][second]) {
                first = up[level][first];
                second = up[level][second];
            }
        }
        return up[0][first];
    }

    int distance(int first, int second) const {
        int ancestor = lca(first, second);
        if (ancestor == -1) return -1;
        return depth[first] + depth[second] - 2 * depth[ancestor];
    }

    bool is_ancestor(int ancestor, int vertex) const {
        return lca(ancestor, vertex) == ancestor;
    }

private:
    void build_component(const vector<vector<int>> &tree, int root, int id) {
        vector<int> stack = {root};
        component[root] = id;
        up[0][root] = root;
        while (!stack.empty()) {
            int vertex = stack.back();
            stack.pop_back();
            for (int to : tree[vertex]) {
                if (component[to] != -1) continue;
                component[to] = id;
                up[0][to] = vertex;
                depth[to] = depth[vertex] + 1;
                stack.push_back(to);
            }
        }
    }
};
