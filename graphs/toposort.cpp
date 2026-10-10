// Kahn topological sort. Returns nullopt when the graph contains a cycle.

optional<vector<int>> topological_sort(const vector<vector<int>> &graph) {
    int n = static_cast<int>(graph.size());
    vector<int> indegree(n, 0);
    for (const vector<int> &edges : graph) {
        for (int to : edges) ++indegree[to];
    }

    vector<int> queue;
    for (int vertex = 0; vertex < n; ++vertex) {
        if (indegree[vertex] == 0) queue.push_back(vertex);
    }
    for (int head = 0; head < static_cast<int>(queue.size()); ++head) {
        int vertex = queue[head];
        for (int to : graph[vertex]) {
            if (--indegree[to] == 0) queue.push_back(to);
        }
    }
    if (static_cast<int>(queue.size()) != n) return nullopt;
    return queue;
}
