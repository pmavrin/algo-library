// Weighted shortest paths. Dijkstra requires non-negative weights.

struct weighted_edge {
    int to;
    int64_t weight;
};

struct shortest_path_result {
    static constexpr int64_t INF = numeric_limits<int64_t>::max() / 4;
    vector<int64_t> distance;
    vector<int> parent;
};

shortest_path_result dijkstra(
    const vector<vector<weighted_edge>> &graph,
    const vector<int> &sources
) {
    int n = static_cast<int>(graph.size());
    shortest_path_result result{vector<int64_t>(n, shortest_path_result::INF),
                                vector<int>(n, -1)};
    priority_queue<pair<int64_t, int>, vector<pair<int64_t, int>>, greater<>> queue;
    for (int source : sources) {
        assert(0 <= source && source < n);
        if (result.distance[source] == 0) continue;
        result.distance[source] = 0;
        result.parent[source] = source;
        queue.push({0, source});
    }

    while (!queue.empty()) {
        auto [distance, vertex] = queue.top();
        queue.pop();
        if (distance != result.distance[vertex]) continue;
        for (weighted_edge edge : graph[vertex]) {
            assert(edge.weight >= 0);
            if (result.distance[edge.to] > distance + edge.weight) {
                result.distance[edge.to] = distance + edge.weight;
                result.parent[edge.to] = vertex;
                queue.push({result.distance[edge.to], edge.to});
            }
        }
    }
    return result;
}

shortest_path_result dijkstra(
    const vector<vector<weighted_edge>> &graph,
    int source
) {
    return dijkstra(graph, vector<int>{source});
}

shortest_path_result zero_one_bfs(
    const vector<vector<weighted_edge>> &graph,
    int source
) {
    int n = static_cast<int>(graph.size());
    assert(0 <= source && source < n);
    shortest_path_result result{vector<int64_t>(n, shortest_path_result::INF),
                                vector<int>(n, -1)};
    deque<int> queue = {source};
    result.distance[source] = 0;
    result.parent[source] = source;

    while (!queue.empty()) {
        int vertex = queue.front();
        queue.pop_front();
        for (weighted_edge edge : graph[vertex]) {
            assert(edge.weight == 0 || edge.weight == 1);
            int64_t next_distance = result.distance[vertex] + edge.weight;
            if (next_distance < result.distance[edge.to]) {
                result.distance[edge.to] = next_distance;
                result.parent[edge.to] = vertex;
                if (edge.weight == 0) queue.push_front(edge.to);
                else queue.push_back(edge.to);
            }
        }
    }
    return result;
}
