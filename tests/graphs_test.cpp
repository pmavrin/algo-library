#include <bits/stdc++.h>

#define int int64_t
#define long int64_t

#include "../graphs/dfs.cpp"
#include "../graphs/dfs_tree.cpp"
#include "../graphs/bfs.cpp"
#include "../graphs/dijkstra.cpp"
#include "../graphs/toposort.cpp"
#include "../graphs/scc.cpp"
#include "../graphs/2sat.cpp"
#include "../graphs/lca.cpp"
#include "../graphs/centroid.cpp"
#include "../graphs/bridges.cpp"
#include "../graphs/mst.cpp"
#include "../graphs/bipartite_matching.cpp"
#include "../graphs/hungarian.cpp"
#include "../graphs/maxflow.cpp"
#include "../graphs/mincost.cpp"
#include "../graphs/mincost_scaling.cpp"

void test_traversals() {
    vector<vector<int>> graph = {{1}, {0, 2}, {1}, {4}, {3}};
    assert(dfs_order(graph, 0) == vector<int>({0, 1, 2}));
    assert(connected_components(graph) == vector<int>({0, 0, 0, 1, 1}));
    bfs_result result = bfs(graph, 0);
    assert(result.distance == vector<int>({0, 1, 2, -1, -1}));

    vector<vector<int>> dag = {{1, 2}, {3}, {3}, {}};
    auto order = topological_sort(dag);
    assert(order.has_value());
    vector<int> position(4);
    for (int i = 0; i < 4; ++i) position[(*order)[i]] = i;
    for (int v = 0; v < 4; ++v) for (int to : dag[v]) assert(position[v] < position[to]);
    dag[3].push_back(0);
    assert(!topological_sort(dag).has_value());
}

void test_shortest_paths(mt19937 &rng) {
    const int64_t INF = shortest_path_result::INF;
    for (int n = 1; n <= 9; ++n) {
        for (int iteration = 0; iteration < 100; ++iteration) {
            vector<vector<weighted_edge>> graph(n);
            vector<vector<int64_t>> distance(n, vector<int64_t>(n, INF));
            for (int i = 0; i < n; ++i) distance[i][i] = 0;
            for (int from = 0; from < n; ++from) {
                for (int to = 0; to < n; ++to) {
                    if (from != to && rng() % 4 == 0) {
                        int64_t weight = rng() % 10;
                        graph[from].push_back({to, weight});
                        distance[from][to] = min(distance[from][to], weight);
                    }
                }
            }
            for (int k = 0; k < n; ++k)
                for (int i = 0; i < n; ++i)
                    for (int j = 0; j < n; ++j)
                        if (distance[i][k] != INF && distance[k][j] != INF)
                            distance[i][j] = min(distance[i][j], distance[i][k] + distance[k][j]);
            for (int source = 0; source < n; ++source) {
                assert(dijkstra(graph, source).distance == distance[source]);
            }
        }
    }

    vector<vector<weighted_edge>> graph = {{{1, 1}, {2, 0}}, {{3, 0}}, {{1, 0}}, {}};
    assert(zero_one_bfs(graph, 0).distance == vector<int64_t>({0, 0, 0, 0}));
}

void test_scc(mt19937 &rng) {
    for (int n = 0; n <= 8; ++n) {
        for (int iteration = 0; iteration < 200; ++iteration) {
            vector<vector<int>> graph(n);
            vector<vector<char>> reachable(n, vector<char>(n, false));
            for (int i = 0; i < n; ++i) reachable[i][i] = true;
            for (int from = 0; from < n; ++from) {
                for (int to = 0; to < n; ++to) {
                    if (rng() % 4 == 0) {
                        graph[from].push_back(to);
                        reachable[from][to] = true;
                    }
                }
            }
            for (int k = 0; k < n; ++k)
                for (int i = 0; i < n; ++i)
                    for (int j = 0; j < n; ++j)
                        reachable[i][j] |= reachable[i][k] && reachable[k][j];

            scc_result result = strongly_connected_components(graph);
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    assert((result.component[i] == result.component[j]) ==
                           bool(reachable[i][j] && reachable[j][i]));
                }
                for (int to : graph[i]) {
                    if (result.component[i] != result.component[to])
                        assert(result.component[i] < result.component[to]);
                }
            }
        }
    }
}

void test_two_sat(mt19937 &rng) {
    for (int variables = 0; variables <= 6; ++variables) {
        for (int iteration = 0; iteration < 300; ++iteration) {
            two_sat solver(variables);
            struct clause { int a; bool av; int b; bool bv; };
            vector<clause> clauses;
            if (variables > 0) {
                int count = rng() % 15;
                for (int i = 0; i < count; ++i) {
                    clause current{int(rng() % variables), bool(rng() & 1),
                                   int(rng() % variables), bool(rng() & 1)};
                    clauses.push_back(current);
                    solver.add_or(current.a, current.av, current.b, current.bv);
                }
            }

            optional<vector<bool>> answer = solver.solve();
            bool exists = false;
            for (int mask = 0; mask < (int64_t(1) << variables); ++mask) {
                bool valid = true;
                for (clause current : clauses) {
                    bool first = bool((mask >> current.a) & 1) == current.av;
                    bool second = bool((mask >> current.b) & 1) == current.bv;
                    valid &= first || second;
                }
                exists |= valid;
            }
            assert(answer.has_value() == exists);
            if (answer) {
                for (clause current : clauses) {
                    assert((*answer)[current.a] == current.av ||
                           (*answer)[current.b] == current.bv);
                }
            }
        }
    }
}

void test_trees(mt19937 &rng) {
    for (int n = 1; n <= 100; ++n) {
        vector<vector<int>> tree(n);
        vector<int> parent(n, 0);
        for (int vertex = 1; vertex < n; ++vertex) {
            parent[vertex] = rng() % vertex;
            tree[vertex].push_back(parent[vertex]);
            tree[parent[vertex]].push_back(vertex);
        }

        tree_dfs info(tree);
        lca_binary_lifting lca(tree);
        centroid_decomposition centroid(tree);
        assert(info.subtree_size[0] == n);
        for (int first = 0; first < n; ++first) {
            for (int second = 0; second < n; ++second) {
                int a = first;
                int b = second;
                while (info.depth[a] > info.depth[b]) a = info.parent[a];
                while (info.depth[b] > info.depth[a]) b = info.parent[b];
                while (a != b) {
                    a = info.parent[a];
                    b = info.parent[b];
                }
                assert(lca.lca(first, second) == a);
                assert(lca.distance(first, second) ==
                       info.depth[first] + info.depth[second] - 2 * info.depth[a]);
            }
        }
        for (int vertex = 0; vertex < n; ++vertex) {
            assert(!centroid.paths[vertex].empty());
            assert(centroid.paths[vertex].back() == make_pair(vertex, int(0)));
            for (auto [center, distance] : centroid.paths[vertex]) {
                assert(lca.distance(vertex, center) == distance);
            }
        }
    }
}

void test_lowlink() {
    undirected_lowlink graph(6);
    graph.add_edge(0, 1);
    graph.add_edge(1, 2);
    graph.add_edge(2, 0);
    int bridge = graph.add_edge(1, 3);
    graph.add_edge(3, 4);
    graph.add_edge(3, 4);
    graph.add_edge(4, 5);
    graph.add_edge(5, 3);
    graph.build();
    assert(graph.bridge_ids == vector<int>({bridge}));
    assert(graph.articulation == vector<char>({0, 1, 0, 1, 0, 0}));
    assert(graph.two_edge_component[0] == graph.two_edge_component[2]);
    assert(graph.two_edge_component[0] != graph.two_edge_component[3]);
}

void test_mst() {
    vector<mst_edge> edges = {{0, 1, 4}, {0, 2, 1}, {2, 1, 2},
                              {1, 3, 1}, {2, 3, 5}};
    mst_result result = minimum_spanning_forest(4, edges);
    assert(result.connected());
    assert(result.weight == 4);
    assert(result.edge_ids.size() == 3);
}

int brute_matching(const vector<vector<char>> &edge, int left, int used) {
    if (left == static_cast<int>(edge.size())) return 0;
    int result = brute_matching(edge, left + 1, used);
    for (int right = 0; right < static_cast<int>(edge[left].size()); ++right) {
        if (edge[left][right] && !(used & (int64_t(1) << right))) {
            result = max(result, 1 + brute_matching(edge, left + 1,
                                                    used | (int64_t(1) << right)));
        }
    }
    return result;
}

void test_matching(mt19937 &rng) {
    for (int left_size = 0; left_size <= 6; ++left_size) {
        for (int right_size = 0; right_size <= 6; ++right_size) {
            for (int iteration = 0; iteration < 50; ++iteration) {
                vector<vector<char>> edge(left_size, vector<char>(right_size));
                bipartite_matching matching(left_size, right_size);
                for (int left = 0; left < left_size; ++left) {
                    for (int right = 0; right < right_size; ++right) {
                        if (rng() % 2) {
                            edge[left][right] = true;
                            matching.add_edge(left, right);
                        }
                    }
                }
                int answer = matching.maximum_matching();
                assert(answer == brute_matching(edge, 0, 0));
                assert(matching.maximum_matching() == answer);
                auto [left_cover, right_cover] = matching.minimum_vertex_cover();
                assert(static_cast<int>(left_cover.size() + right_cover.size()) == answer);
            }
        }
    }
}

int64_t brute_assignment(const vector<vector<int64_t>> &cost, int row, int used) {
    if (row == static_cast<int>(cost.size())) return 0;
    int64_t result = numeric_limits<int64_t>::max() / 4;
    for (int column = 0; column < static_cast<int>(cost[row].size()); ++column) {
        if (!(used & (int64_t(1) << column))) {
            result = min(result, cost[row][column] +
                                  brute_assignment(cost, row + 1,
                                                   used | (int64_t(1) << column)));
        }
    }
    return result;
}

void test_hungarian(mt19937 &rng) {
    for (int rows = 0; rows <= 5; ++rows) {
        for (int columns = max<int>(1, rows); columns <= 6; ++columns) {
            for (int iteration = 0; iteration < 50; ++iteration) {
                vector<vector<int64_t>> cost(rows, vector<int64_t>(columns));
                for (auto &row : cost) for (int64_t &value : row) value = int(rng() % 31) - 15;
                auto [answer, assignment] = hungarian(cost);
                assert(answer == brute_assignment(cost, 0, 0));
                set<int> used(assignment.begin(), assignment.end());
                assert(used.size() == assignment.size());
            }
        }
    }
}

void test_maxflow(mt19937 &rng) {
    for (int n = 2; n <= 7; ++n) {
        for (int iteration = 0; iteration < 100; ++iteration) {
            maxflow flow(n);
            struct input_edge { int from, to; int64_t capacity; };
            vector<input_edge> edges;
            for (int from = 0; from < n; ++from) {
                for (int to = 0; to < n; ++to) {
                    if (from != to && rng() % 5 == 0) {
                        int64_t capacity = rng() % 8;
                        edges.push_back({from, to, capacity});
                        flow.add_edge(from, to, capacity);
                    }
                }
            }
            int64_t expected = numeric_limits<int64_t>::max();
            for (int mask = 0; mask < (int64_t(1) << n); ++mask) {
                if (!(mask & 1) || (mask & (int64_t(1) << (n - 1)))) continue;
                int64_t cut = 0;
                for (input_edge edge : edges) {
                    if ((mask & (int64_t(1) << edge.from)) &&
                        !(mask & (int64_t(1) << edge.to))) cut += edge.capacity;
                }
                expected = min(expected, cut);
            }
            assert(flow.max_flow(0, n - 1) == expected);
        }
    }
}

min_cost_flow sample_min_cost_flow() {
    min_cost_flow flow(4);
    flow.add_edge(0, 1, 2, 1);
    flow.add_edge(0, 2, 1, 0);
    flow.add_edge(2, 1, 1, -2);
    flow.add_edge(1, 3, 2, 1);
    flow.add_edge(2, 3, 1, 3);
    return flow;
}

void test_min_cost_flow() {
    auto limited = sample_min_cost_flow();
    assert(limited.flow(0, 3, 2) == make_pair(int64_t(2), int64_t(1)));
    auto full = sample_min_cost_flow();
    assert(full.flow(0, 3) == make_pair(int64_t(3), int64_t(7)));
}

struct brute_cost_edge {
    int from;
    int to;
    int capacity;
    int cost;
};

optional<int64_t> brute_fixed_flow(
    int n,
    const vector<brute_cost_edge> &edges,
    int source,
    int sink,
    int amount
) {
    vector<int> balance(n, 0);
    balance[source] += amount;
    balance[sink] -= amount;
    optional<int64_t> answer;
    function<void(int, int64_t)> search = [&](int index, int64_t cost) {
        if (index == static_cast<int>(edges.size())) {
            if (all_of(balance.begin(), balance.end(), [](int value) { return value == 0; }) &&
                (!answer || cost < *answer)) answer = cost;
            return;
        }
        brute_cost_edge edge = edges[index];
        for (int flow = 0; flow <= edge.capacity; ++flow) {
            balance[edge.from] -= flow;
            balance[edge.to] += flow;
            search(index + 1, cost + int64_t(flow) * edge.cost);
            balance[edge.from] += flow;
            balance[edge.to] -= flow;
        }
    };
    search(0, 0);
    return answer;
}

void test_min_cost_scaling(mt19937 &rng) {
    {
        min_cost_scaling flow(4);
        flow.add_edge(0, 1, 2, 1);
        flow.add_edge(0, 2, 1, 0);
        flow.add_edge(2, 1, 1, -2);
        flow.add_edge(1, 3, 2, 1);
        flow.add_edge(2, 3, 1, 3);
        flow.require_flow(0, 3, 3);
        assert(flow.solve() == optional<int64_t>(7));
    }
    {
        min_cost_scaling circulation(2);
        int first = circulation.add_edge(0, 1, 5, -2);
        int second = circulation.add_edge(1, 0, 3, 0);
        assert(circulation.solve() == optional<int64_t>(-6));
        assert(circulation.edge_flow(first) == 3);
        assert(circulation.edge_flow(second) == 3);
    }
    {
        min_cost_scaling lower_bounds(2);
        lower_bounds.add_edge(0, 1, 2, 5, 3);
        lower_bounds.add_edge(1, 0, 2, 1);
        assert(lower_bounds.solve() == optional<int64_t>(8));

        min_cost_scaling impossible(2);
        impossible.add_edge(0, 1, 2, 5, 3);
        assert(!impossible.solve().has_value());
    }

    for (int n = 2; n <= 5; ++n) {
        for (int iteration = 0; iteration < 500; ++iteration) {
            int edge_count = rng() % 7;
            vector<brute_cost_edge> edges;
            min_cost_scaling scaling(n);
            for (int i = 0; i < edge_count; ++i) {
                int from = rng() % n;
                int to = rng() % n;
                if (from == to) {
                    --i;
                    continue;
                }
                int capacity = rng() % 3;
                int cost = int(rng() % 9) - 4;
                edges.push_back({from, to, capacity, cost});
                scaling.add_edge(from, to, capacity, cost);
            }
            int amount = rng() % 3;
            scaling.require_flow(0, n - 1, amount);
            assert(scaling.solve() == brute_fixed_flow(n, edges, 0, n - 1, amount));
        }
    }
}

signed main() {
    mt19937 rng(712367);
    test_traversals();
    test_shortest_paths(rng);
    test_scc(rng);
    test_two_sat(rng);
    test_trees(rng);
    test_lowlink();
    test_mst();
    test_matching(rng);
    test_hungarian(rng);
    test_maxflow(rng);
    test_min_cost_flow();
    test_min_cost_scaling(rng);
}
