// Dinic maximum flow with int64_t capacities.

struct maxflow {
    struct edge {
        int from;
        int to;
        int64_t capacity;
        int64_t flow;
    };

    int n;
    vector<edge> edges;
    vector<vector<int>> graph;
    vector<int> level;
    vector<int> pointer;

    explicit maxflow(int n) : n(n), graph(n), level(n), pointer(n) {
        assert(n >= 0);
    }

    int add_edge(int from, int to, int64_t capacity) {
        assert(0 <= from && from < n && 0 <= to && to < n && capacity >= 0);
        int id = static_cast<int>(edges.size());
        edges.push_back({from, to, capacity, 0});
        edges.push_back({to, from, 0, 0});
        graph[from].push_back(id);
        graph[to].push_back(id ^ 1);
        return id;
    }

    int64_t max_flow(
        int source,
        int sink,
        int64_t limit = numeric_limits<int64_t>::max()
    ) {
        assert(0 <= source && source < n && 0 <= sink && sink < n);
        if (source == sink) return 0;
        int64_t result = 0;
        while (result < limit && build_levels(source, sink)) {
            fill(pointer.begin(), pointer.end(), 0);
            while (result < limit) {
                int64_t pushed = push(source, sink, limit - result);
                if (pushed == 0) break;
                result += pushed;
            }
        }
        return result;
    }

    vector<char> min_cut(int source) const {
        assert(0 <= source && source < n);
        vector<char> reachable(n, false);
        vector<int> queue = {source};
        reachable[source] = true;
        for (int head = 0; head < static_cast<int>(queue.size()); ++head) {
            int vertex = queue[head];
            for (int id : graph[vertex]) {
                const edge &e = edges[id];
                if (!reachable[e.to] && e.flow < e.capacity) {
                    reachable[e.to] = true;
                    queue.push_back(e.to);
                }
            }
        }
        return reachable;
    }

private:
    bool build_levels(int source, int sink) {
        fill(level.begin(), level.end(), -1);
        vector<int> queue = {source};
        level[source] = 0;
        for (int head = 0; head < static_cast<int>(queue.size()); ++head) {
            int vertex = queue[head];
            for (int id : graph[vertex]) {
                const edge &e = edges[id];
                if (level[e.to] == -1 && e.flow < e.capacity) {
                    level[e.to] = level[vertex] + 1;
                    queue.push_back(e.to);
                }
            }
        }
        return level[sink] != -1;
    }

    int64_t push(int vertex, int sink, int64_t pushed) {
        if (vertex == sink || pushed == 0) return pushed;
        for (int &edge_index = pointer[vertex];
             edge_index < static_cast<int>(graph[vertex].size());
             ++edge_index) {
            int id = graph[vertex][edge_index];
            edge &e = edges[id];
            if (level[e.to] != level[vertex] + 1 || e.flow == e.capacity) continue;
            int64_t delta = push(e.to, sink, min(pushed, e.capacity - e.flow));
            if (delta == 0) continue;
            e.flow += delta;
            edges[id ^ 1].flow -= delta;
            return delta;
        }
        return 0;
    }
};
