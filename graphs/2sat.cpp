// 2-SAT. A literal is represented by (variable, value).

struct two_sat {
    int variables;
    vector<vector<int>> graph;

    explicit two_sat(int variables)
        : variables(variables), graph(2 * variables) {
        assert(variables >= 0);
    }

    int literal(int variable, bool value) const {
        assert(0 <= variable && variable < variables);
        return 2 * variable + value;
    }

    void add_implication(int a, bool a_value, int b, bool b_value) {
        int from = literal(a, a_value);
        int to = literal(b, b_value);
        graph[from].push_back(to);
        graph[to ^ 1].push_back(from ^ 1);
    }

    void add_or(int a, bool a_value, int b, bool b_value) {
        add_implication(a, !a_value, b, b_value);
    }

    void add_xor(int a, bool a_value, int b, bool b_value) {
        add_or(a, a_value, b, b_value);
        add_or(a, !a_value, b, !b_value);
    }

    void add_equivalence(int a, bool a_value, int b, bool b_value) {
        add_implication(a, a_value, b, b_value);
        add_implication(b, b_value, a, a_value);
    }

    void force(int variable, bool value) {
        add_implication(variable, !value, variable, value);
    }

    optional<vector<bool>> solve() const {
        int n = 2 * variables;
        vector<vector<int>> reverse_graph(n);
        for (int vertex = 0; vertex < n; ++vertex) {
            for (int to : graph[vertex]) reverse_graph[to].push_back(vertex);
        }

        vector<char> visited(n, false);
        vector<int> order;
        function<void(int)> dfs1 = [&](int vertex) {
            visited[vertex] = true;
            for (int to : graph[vertex]) if (!visited[to]) dfs1(to);
            order.push_back(vertex);
        };
        for (int vertex = 0; vertex < n; ++vertex) {
            if (!visited[vertex]) dfs1(vertex);
        }

        vector<int> component(n, -1);
        function<void(int, int)> dfs2 = [&](int vertex, int id) {
            component[vertex] = id;
            for (int to : reverse_graph[vertex]) {
                if (component[to] == -1) dfs2(to, id);
            }
        };
        reverse(order.begin(), order.end());
        int components = 0;
        for (int vertex : order) {
            if (component[vertex] == -1) dfs2(vertex, components++);
        }

        vector<bool> assignment(variables);
        for (int variable = 0; variable < variables; ++variable) {
            int false_literal = literal(variable, false);
            int true_literal = literal(variable, true);
            if (component[false_literal] == component[true_literal]) return nullopt;
            assignment[variable] = component[false_literal] < component[true_literal];
        }
        return assignment;
    }
};
