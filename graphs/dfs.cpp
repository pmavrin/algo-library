// Iterative DFS utilities for a graph with vertices [0, n).

vector<int> dfs_order(const vector<vector<int>> &graph, int start) {
    int n = static_cast<int>(graph.size());
    assert(0 <= start && start < n);
    vector<int> order;
    vector<char> visited(n, false);
    vector<int> stack = {start};
    visited[start] = true;

    while (!stack.empty()) {
        int vertex = stack.back();
        stack.pop_back();
        order.push_back(vertex);
        for (int i = static_cast<int>(graph[vertex].size()) - 1; i >= 0; --i) {
            int to = graph[vertex][i];
            if (!visited[to]) {
                visited[to] = true;
                stack.push_back(to);
            }
        }
    }
    return order;
}

vector<int> connected_components(const vector<vector<int>> &graph) {
    int n = static_cast<int>(graph.size());
    vector<int> component(n, -1);
    int components = 0;

    for (int start = 0; start < n; ++start) {
        if (component[start] != -1) continue;
        vector<int> stack = {start};
        component[start] = components;
        while (!stack.empty()) {
            int vertex = stack.back();
            stack.pop_back();
            for (int to : graph[vertex]) {
                if (component[to] == -1) {
                    component[to] = components;
                    stack.push_back(to);
                }
            }
        }
        ++components;
    }
    return component;
}
