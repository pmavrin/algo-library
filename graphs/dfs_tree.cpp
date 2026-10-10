// Parent, depth, subtree sizes and Euler entry/exit times for a forest.

struct tree_dfs {
    int timer = 0;
    vector<int> parent;
    vector<int> depth;
    vector<int> subtree_size;
    vector<int> tin;
    vector<int> tout;
    vector<int> preorder;

    explicit tree_dfs(const vector<vector<int>> &tree, int root = 0) {
        int n = static_cast<int>(tree.size());
        parent.assign(n, -1);
        depth.assign(n, 0);
        subtree_size.assign(n, 0);
        tin.assign(n, -1);
        tout.assign(n, -1);
        if (n == 0) return;
        assert(0 <= root && root < n);
        run(tree, root, root);
        for (int vertex = 0; vertex < n; ++vertex) {
            if (parent[vertex] == -1) run(tree, vertex, vertex);
        }
    }

    bool is_ancestor(int ancestor, int vertex) const {
        return tin[ancestor] <= tin[vertex] && tout[vertex] <= tout[ancestor];
    }

private:
    void run(const vector<vector<int>> &tree, int vertex, int root) {
        struct event {
            int vertex;
            int parent;
            bool exit;
        };
        vector<event> stack = {{vertex, root, false}};
        parent[vertex] = root;

        while (!stack.empty()) {
            event current = stack.back();
            stack.pop_back();
            int v = current.vertex;
            if (current.exit) {
                tout[v] = timer++;
                subtree_size[v] = 1;
                for (int to : tree[v]) {
                    if (parent[to] == v && to != v) subtree_size[v] += subtree_size[to];
                }
                continue;
            }

            tin[v] = timer++;
            preorder.push_back(v);
            stack.push_back({v, current.parent, true});
            for (int i = static_cast<int>(tree[v].size()) - 1; i >= 0; --i) {
                int to = tree[v][i];
                if (to == current.parent || parent[to] != -1) continue;
                parent[to] = v;
                depth[to] = depth[v] + 1;
                stack.push_back({to, v, false});
            }
        }
    }
};
