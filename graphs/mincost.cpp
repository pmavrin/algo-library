// Successive shortest augmenting paths with Johnson potentials.
// Returns the maximum possible flow up to limit and its minimum cost.

struct min_cost_flow {
    struct edge {
        int from;
        int to;
        int64_t capacity;
        int64_t flow;
        int64_t cost;
    };

    static constexpr int64_t INF = numeric_limits<int64_t>::max() / 4;

    int n;
    vector<edge> edges;
    vector<vector<int>> graph;

    explicit min_cost_flow(int n) : n(n), graph(n) {
        assert(n >= 0);
    }

    int add_edge(int from, int to, int64_t capacity, int64_t cost) {
        assert(0 <= from && from < n && 0 <= to && to < n && capacity >= 0);
        int id = static_cast<int>(edges.size());
        edges.push_back({from, to, capacity, 0, cost});
        edges.push_back({to, from, 0, 0, -cost});
        graph[from].push_back(id);
        graph[to].push_back(id ^ 1);
        return id;
    }

    pair<int64_t, int64_t> flow(
        int source,
        int sink,
        int64_t limit = numeric_limits<int64_t>::max()
    ) {
        assert(0 <= source && source < n && 0 <= sink && sink < n);
        if (source == sink) return {0, 0};

        vector<int64_t> potential = initial_potentials(source);
        vector<int64_t> distance(n);
        vector<int> previous(n);
        int64_t total_flow = 0;
        int64_t total_cost = 0;

        while (total_flow < limit) {
            fill(distance.begin(), distance.end(), INF);
            fill(previous.begin(), previous.end(), -1);
            priority_queue<pair<int64_t, int>, vector<pair<int64_t, int>>, greater<>> queue;
            distance[source] = 0;
            queue.push({0, source});

            while (!queue.empty()) {
                auto [current_distance, vertex] = queue.top();
                queue.pop();
                if (current_distance != distance[vertex]) continue;
                for (int id : graph[vertex]) {
                    const edge &e = edges[id];
                    if (e.flow == e.capacity) continue;
                    int64_t reduced_cost = e.cost + potential[vertex] - potential[e.to];
                    int64_t next_distance = current_distance + reduced_cost;
                    if (next_distance < distance[e.to]) {
                        distance[e.to] = next_distance;
                        previous[e.to] = id;
                        queue.push({next_distance, e.to});
                    }
                }
            }

            if (previous[sink] == -1) break;
            for (int vertex = 0; vertex < n; ++vertex) {
                if (distance[vertex] != INF) potential[vertex] += distance[vertex];
            }

            int64_t pushed = limit - total_flow;
            for (int vertex = sink; vertex != source; ) {
                const edge &e = edges[previous[vertex]];
                pushed = min(pushed, e.capacity - e.flow);
                vertex = e.from;
            }
            for (int vertex = sink; vertex != source; ) {
                int id = previous[vertex];
                edge &e = edges[id];
                e.flow += pushed;
                edges[id ^ 1].flow -= pushed;
                total_cost += pushed * e.cost;
                vertex = e.from;
            }
            total_flow += pushed;
        }
        return {total_flow, total_cost};
    }

private:
    vector<int64_t> initial_potentials(int source) const {
        vector<int64_t> distance(n, INF);
        distance[source] = 0;
        for (int iteration = 0; iteration + 1 < n; ++iteration) {
            bool changed = false;
            for (const edge &e : edges) {
                if (e.flow == e.capacity || distance[e.from] == INF) continue;
                if (distance[e.to] > distance[e.from] + e.cost) {
                    distance[e.to] = distance[e.from] + e.cost;
                    changed = true;
                }
            }
            if (!changed) break;
        }
        for (int64_t &value : distance) if (value == INF) value = 0;
        return distance;
    }
};
