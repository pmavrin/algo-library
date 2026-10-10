// Bridges, articulation points and 2-edge-connected components.
// Add every undirected edge once with add_edge(). Parallel edges are supported.

struct undirected_lowlink {
    struct edge {
        int from;
        int to;
    };

    int n;
    int timer = 0;
    vector<edge> edges;
    vector<vector<pair<int, int>>> graph;
    vector<int> tin;
    vector<int> low;
    vector<char> articulation;
    vector<int> bridge_ids;
    vector<int> two_edge_component;

    explicit undirected_lowlink(int n) : n(n), graph(n) {
        assert(n >= 0);
    }

    int add_edge(int from, int to) {
        assert(0 <= from && from < n && 0 <= to && to < n);
        int id = static_cast<int>(edges.size());
        edges.push_back({from, to});
        graph[from].push_back({to, id});
        graph[to].push_back({from, id});
        return id;
    }

    void build() {
        timer = 0;
        tin.assign(n, -1);
        low.assign(n, -1);
        articulation.assign(n, false);
        bridge_ids.clear();
        for (int vertex = 0; vertex < n; ++vertex) {
            if (tin[vertex] == -1) dfs(vertex, -1);
        }
        sort(bridge_ids.begin(), bridge_ids.end());

        vector<char> is_bridge(edges.size(), false);
        for (int id : bridge_ids) is_bridge[id] = true;
        two_edge_component.assign(n, -1);
        int components = 0;
        for (int start = 0; start < n; ++start) {
            if (two_edge_component[start] != -1) continue;
            vector<int> stack = {start};
            two_edge_component[start] = components;
            while (!stack.empty()) {
                int vertex = stack.back();
                stack.pop_back();
                for (auto [to, id] : graph[vertex]) {
                    if (!is_bridge[id] && two_edge_component[to] == -1) {
                        two_edge_component[to] = components;
                        stack.push_back(to);
                    }
                }
            }
            ++components;
        }
    }

private:
    void dfs(int vertex, int parent_edge) {
        tin[vertex] = low[vertex] = timer++;
        int children = 0;
        for (auto [to, id] : graph[vertex]) {
            if (id == parent_edge) continue;
            if (tin[to] != -1) {
                low[vertex] = min(low[vertex], tin[to]);
            } else {
                dfs(to, id);
                low[vertex] = min(low[vertex], low[to]);
                if (low[to] > tin[vertex]) bridge_ids.push_back(id);
                if (parent_edge != -1 && low[to] >= tin[vertex]) articulation[vertex] = true;
                ++children;
            }
        }
        if (parent_edge == -1 && children > 1) articulation[vertex] = true;
    }
};
