// Unweighted shortest paths from one or several sources.

struct bfs_result {
    vector<int> distance;
    vector<int> parent;
};

bfs_result bfs(const vector<vector<int>> &graph, const vector<int> &sources) {
    int n = static_cast<int>(graph.size());
    bfs_result result{vector<int>(n, -1), vector<int>(n, -1)};
    vector<int> queue;
    for (int source : sources) {
        assert(0 <= source && source < n);
        if (result.distance[source] != -1) continue;
        result.distance[source] = 0;
        result.parent[source] = source;
        queue.push_back(source);
    }
    for (int head = 0; head < static_cast<int>(queue.size()); ++head) {
        int vertex = queue[head];
        for (int to : graph[vertex]) {
            if (result.distance[to] == -1) {
                result.distance[to] = result.distance[vertex] + 1;
                result.parent[to] = vertex;
                queue.push_back(to);
            }
        }
    }
    return result;
}

bfs_result bfs(const vector<vector<int>> &graph, int source) {
    return bfs(graph, vector<int>{source});
}
