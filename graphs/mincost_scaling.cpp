// Cost-scaling push-relabel for minimum-cost circulation.
//
// Supports lower bounds and vertex supplies. A positive supply means that the
// vertex must send this amount; a negative supply means that it must receive it.
// Costs and capacities are integral. The instance is modified by solve().

struct min_cost_scaling {
    struct edge {
        int from;
        int to;
        int64_t capacity;
        int64_t flow;
        int64_t cost;
    };

    int n;
    vector<edge> edges;
    vector<vector<int>> graph;
    vector<int64_t> balance;
    vector<int64_t> lower_bound;
    int64_t total_cost = 0;
    bool solved = false;

    explicit min_cost_scaling(int n)
        : n(n), graph(n), balance(n, 0) {
        assert(n >= 0);
    }

    int add_edge(int from, int to, int64_t capacity, int64_t cost) {
        return add_edge(from, to, 0, capacity, cost);
    }

    int add_edge(
        int from,
        int to,
        int64_t minimum_flow,
        int64_t maximum_flow,
        int64_t cost
    ) {
        assert(!solved);
        assert(0 <= from && from < n && 0 <= to && to < n);
        assert(0 <= minimum_flow && minimum_flow <= maximum_flow);
        int id = static_cast<int>(edges.size());
        edges.push_back({from, to, maximum_flow - minimum_flow, 0, cost});
        edges.push_back({to, from, 0, 0, -cost});
        graph[from].push_back(id);
        graph[to].push_back(id ^ 1);
        lower_bound.push_back(minimum_flow);
        balance[from] -= minimum_flow;
        balance[to] += minimum_flow;
        total_cost += minimum_flow * cost;
        return id;
    }

    void add_supply(int vertex, int64_t supply) {
        assert(!solved);
        assert(0 <= vertex && vertex < n);
        balance[vertex] += supply;
    }

    void require_flow(int source, int sink, int64_t amount) {
        assert(amount >= 0);
        add_supply(source, amount);
        add_supply(sink, -amount);
    }

    int64_t edge_flow(int id) const {
        assert(0 <= id && id < static_cast<int>(edges.size()) && id % 2 == 0);
        return lower_bound[id / 2] + edges[id].flow;
    }

    optional<int64_t> solve() {
        assert(!solved);
        solved = true;
        if (!make_feasible()) return nullopt;
        optimize_cost();
        return total_cost;
    }

    optional<int64_t> solve_flow(int source, int sink, int64_t amount) {
        require_flow(source, sink, amount);
        return solve();
    }

private:
    struct feasibility_flow {
        struct local_edge {
            int to;
            int reverse;
            int64_t capacity;
            int64_t flow;
        };

        vector<vector<local_edge>> graph;
        vector<int> level;
        vector<int> pointer;

        explicit feasibility_flow(int n) : graph(n), level(n), pointer(n) {}

        pair<int, int> add_edge(int from, int to, int64_t capacity) {
            int index = static_cast<int>(graph[from].size());
            int reverse = static_cast<int>(graph[to].size());
            graph[from].push_back({to, reverse, capacity, 0});
            graph[to].push_back({from, index, 0, 0});
            return {from, index};
        }

        int64_t max_flow(int source, int sink) {
            int64_t result = 0;
            while (build_levels(source, sink)) {
                fill(pointer.begin(), pointer.end(), 0);
                while (int64_t pushed = push(
                    source,
                    sink,
                    numeric_limits<int64_t>::max()
                )) result += pushed;
            }
            return result;
        }

        bool build_levels(int source, int sink) {
            fill(level.begin(), level.end(), -1);
            vector<int> queue = {source};
            level[source] = 0;
            for (int head = 0; head < static_cast<int>(queue.size()); ++head) {
                int vertex = queue[head];
                for (const local_edge &e : graph[vertex]) {
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
            for (int &index = pointer[vertex];
                 index < static_cast<int>(graph[vertex].size());
                 ++index) {
                local_edge &e = graph[vertex][index];
                if (level[e.to] != level[vertex] + 1 || e.flow == e.capacity) continue;
                int64_t delta = push(e.to, sink, min(pushed, e.capacity - e.flow));
                if (delta == 0) continue;
                e.flow += delta;
                graph[e.to][e.reverse].flow -= delta;
                return delta;
            }
            return 0;
        }
    };

    bool make_feasible() {
        int64_t balance_sum = accumulate(balance.begin(), balance.end(), int64_t(0));
        if (balance_sum != 0) return false;

        int source = n;
        int sink = n + 1;
        feasibility_flow flow(n + 2);
        vector<pair<int, int>> location(edges.size() / 2);
        for (int id = 0; id < static_cast<int>(edges.size()); id += 2) {
            location[id / 2] = flow.add_edge(
                edges[id].from,
                edges[id].to,
                edges[id].capacity
            );
        }

        int64_t required = 0;
        for (int vertex = 0; vertex < n; ++vertex) {
            if (balance[vertex] > 0) {
                flow.add_edge(source, vertex, balance[vertex]);
                required += balance[vertex];
            } else if (balance[vertex] < 0) {
                flow.add_edge(vertex, sink, -balance[vertex]);
            }
        }
        if (flow.max_flow(source, sink) != required) return false;

        for (int id = 0; id < static_cast<int>(edges.size()); id += 2) {
            auto [vertex, index] = location[id / 2];
            int64_t pushed = flow.graph[vertex][index].flow;
            edges[id].flow += pushed;
            edges[id ^ 1].flow -= pushed;
            total_cost += pushed * edges[id].cost;
        }
        return true;
    }

    int64_t reduced_cost(int id, const vector<int64_t> &potential, int64_t scale) const {
        const edge &e = edges[id];
        return e.cost * scale + potential[e.from] - potential[e.to];
    }

    void push_edge(int id, int64_t amount, vector<int64_t> &excess) {
        edge &e = edges[id];
        e.flow += amount;
        edges[id ^ 1].flow -= amount;
        excess[e.from] -= amount;
        excess[e.to] += amount;
        total_cost += amount * e.cost;
    }

    void optimize_cost() {
        if (n == 0 || edges.empty()) return;
        const int64_t scale = n + 1;
        int64_t maximum_scaled_cost = 0;
        for (const edge &e : edges) {
            assert(e.cost != numeric_limits<int64_t>::min());
            int64_t absolute_cost = abs(e.cost);
            assert(absolute_cost <= numeric_limits<int64_t>::max() / scale);
            maximum_scaled_cost = max(maximum_scaled_cost, absolute_cost * scale);
        }

        int64_t epsilon = 1;
        while (epsilon < maximum_scaled_cost) {
            if (epsilon > numeric_limits<int64_t>::max() / 4) {
                epsilon = maximum_scaled_cost;
                break;
            }
            epsilon *= 4;
        }

        vector<int64_t> potential(n, 0);
        vector<int64_t> excess(n, 0);
        vector<int> pointer(n, 0);
        vector<char> active(n, false);

        for (; epsilon >= 1; epsilon /= 4) {
            fill(excess.begin(), excess.end(), 0);
            fill(pointer.begin(), pointer.end(), 0);
            fill(active.begin(), active.end(), false);

            for (int id = 0; id < static_cast<int>(edges.size()); ++id) {
                int64_t residual = edges[id].capacity - edges[id].flow;
                if (residual > 0 && reduced_cost(id, potential, scale) < 0) {
                    push_edge(id, residual, excess);
                }
            }

            deque<int> queue;
            for (int vertex = 0; vertex < n; ++vertex) {
                if (excess[vertex] > 0) {
                    queue.push_back(vertex);
                    active[vertex] = true;
                }
            }

            while (!queue.empty()) {
                int vertex = queue.front();
                queue.pop_front();
                active[vertex] = false;

                while (excess[vertex] > 0) {
                    if (pointer[vertex] == static_cast<int>(graph[vertex].size())) {
                        int64_t minimum = numeric_limits<int64_t>::max();
                        for (int id : graph[vertex]) {
                            if (edges[id].flow < edges[id].capacity) {
                                minimum = min(minimum, reduced_cost(id, potential, scale));
                            }
                        }
                        assert(minimum != numeric_limits<int64_t>::max());
                        potential[vertex] -= minimum + epsilon;
                        pointer[vertex] = 0;
                        continue;
                    }

                    int id = graph[vertex][pointer[vertex]];
                    edge &e = edges[id];
                    if (e.flow == e.capacity || reduced_cost(id, potential, scale) >= 0) {
                        ++pointer[vertex];
                        continue;
                    }

                    int to = e.to;
                    int64_t old_excess = excess[to];
                    int64_t pushed = min(excess[vertex], e.capacity - e.flow);
                    push_edge(id, pushed, excess);
                    if (old_excess <= 0 && excess[to] > 0 && !active[to]) {
                        queue.push_back(to);
                        active[to] = true;
                    }
                }
            }

            if (epsilon == 1) break;
        }
    }
};
