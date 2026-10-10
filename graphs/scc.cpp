// Kosaraju SCC. Component numbers follow a topological order of condensation.

struct scc_result {
    int count = 0;
    vector<int> component;
    vector<vector<int>> groups;
    vector<vector<int>> condensation;
};

scc_result strongly_connected_components(const vector<vector<int>> &graph) {
    int n = static_cast<int>(graph.size());
    vector<vector<int>> reverse_graph(n);
    for (int vertex = 0; vertex < n; ++vertex) {
        for (int to : graph[vertex]) reverse_graph[to].push_back(vertex);
    }

    vector<char> visited(n, false);
    vector<int> order;
    for (int start = 0; start < n; ++start) {
        if (visited[start]) continue;
        vector<pair<int, int>> stack = {{start, 0}};
        visited[start] = true;
        while (!stack.empty()) {
            int vertex = stack.back().first;
            int &next_edge = stack.back().second;
            if (next_edge < static_cast<int>(graph[vertex].size())) {
                int to = graph[vertex][next_edge++];
                if (!visited[to]) {
                    visited[to] = true;
                    stack.push_back({to, 0});
                }
            } else {
                order.push_back(vertex);
                stack.pop_back();
            }
        }
    }

    scc_result result;
    result.component.assign(n, -1);
    reverse(order.begin(), order.end());
    for (int start : order) {
        if (result.component[start] != -1) continue;
        result.groups.push_back({});
        vector<int> stack = {start};
        result.component[start] = result.count;
        while (!stack.empty()) {
            int vertex = stack.back();
            stack.pop_back();
            result.groups.back().push_back(vertex);
            for (int to : reverse_graph[vertex]) {
                if (result.component[to] == -1) {
                    result.component[to] = result.count;
                    stack.push_back(to);
                }
            }
        }
        ++result.count;
    }

    result.condensation.assign(result.count, {});
    for (int vertex = 0; vertex < n; ++vertex) {
        for (int to : graph[vertex]) {
            int from_component = result.component[vertex];
            int to_component = result.component[to];
            if (from_component != to_component) {
                result.condensation[from_component].push_back(to_component);
            }
        }
    }
    for (vector<int> &edges : result.condensation) {
        sort(edges.begin(), edges.end());
        edges.erase(unique(edges.begin(), edges.end()), edges.end());
    }
    return result;
}
