// Kruskal minimum spanning forest.

struct mst_edge {
    int from;
    int to;
    int64_t weight;
    int id = -1;
};

struct mst_result {
    int64_t weight = 0;
    vector<int> edge_ids;
    int components = 0;

    bool connected() const {
        return components <= 1;
    }
};

mst_result minimum_spanning_forest(int n, vector<mst_edge> edges) {
    assert(n >= 0);
    vector<int> parent(n);
    vector<int> size(n, 1);
    iota(parent.begin(), parent.end(), 0);

    auto find = [&](int vertex) {
        int root = vertex;
        while (parent[root] != root) root = parent[root];
        while (parent[vertex] != vertex) {
            int next = parent[vertex];
            parent[vertex] = root;
            vertex = next;
        }
        return root;
    };

    for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
        if (edges[i].id == -1) edges[i].id = i;
    }
    sort(edges.begin(), edges.end(), [](const mst_edge &left, const mst_edge &right) {
        return left.weight < right.weight;
    });

    mst_result result;
    result.components = n;
    for (const mst_edge &edge : edges) {
        assert(0 <= edge.from && edge.from < n && 0 <= edge.to && edge.to < n);
        int first = find(edge.from);
        int second = find(edge.to);
        if (first == second) continue;
        if (size[first] < size[second]) swap(first, second);
        parent[second] = first;
        size[first] += size[second];
        result.weight += edge.weight;
        result.edge_ids.push_back(edge.id);
        --result.components;
    }
    return result;
}
