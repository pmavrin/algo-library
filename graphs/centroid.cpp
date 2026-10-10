// Centroid decomposition of a tree or forest.
// paths[v] contains (centroid, distance) from top to bottom of the decomposition.

struct centroid_decomposition {
    const vector<vector<int>> &tree;
    vector<int> parent;
    vector<int> level;
    vector<int> subtree_size;
    vector<char> removed;
    vector<vector<pair<int, int>>> paths;

    explicit centroid_decomposition(const vector<vector<int>> &tree)
        : tree(tree) {
        int n = static_cast<int>(tree.size());
        parent.assign(n, -1);
        level.assign(n, -1);
        subtree_size.assign(n, 0);
        removed.assign(n, false);
        paths.resize(n);
        for (int vertex = 0; vertex < n; ++vertex) {
            if (!removed[vertex]) build(vertex, -1, 0);
        }
    }

private:
    int calculate_size(int vertex, int previous) {
        subtree_size[vertex] = 1;
        for (int to : tree[vertex]) {
            if (to != previous && !removed[to]) {
                subtree_size[vertex] += calculate_size(to, vertex);
            }
        }
        return subtree_size[vertex];
    }

    int find_centroid(int vertex, int previous, int total) {
        for (int to : tree[vertex]) {
            if (to != previous && !removed[to] && subtree_size[to] > total / 2) {
                return find_centroid(to, vertex, total);
            }
        }
        return vertex;
    }

    void add_paths(int vertex, int previous, int centroid, int distance) {
        paths[vertex].push_back({centroid, distance});
        for (int to : tree[vertex]) {
            if (to != previous && !removed[to]) {
                add_paths(to, vertex, centroid, distance + 1);
            }
        }
    }

    int build(int entry, int centroid_parent, int centroid_level) {
        int total = calculate_size(entry, -1);
        int centroid = find_centroid(entry, -1, total);
        parent[centroid] = centroid_parent;
        level[centroid] = centroid_level;
        add_paths(centroid, -1, centroid, 0);
        removed[centroid] = true;
        for (int to : tree[centroid]) {
            if (!removed[to]) build(to, centroid, centroid_level + 1);
        }
        return centroid;
    }
};
