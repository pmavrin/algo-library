#include <bits/stdc++.h>
using namespace std;

#ifndef SUFFIX_TREE_NO_INT_MACROS
#define int int64_t
#define long int64_t
#endif

#include "../strings/suffix_tree.cpp"

vector<int> slow_occurrences(const string &s, const string &pattern) {
    vector<int> result;
    for (size_t i = 0; i + pattern.size() <= s.size(); ++i) {
        if (s.compare(i, pattern.size(), pattern) == 0) result.push_back(static_cast<int>(i));
    }
    return result;
}

// Build the quadratic suffix trie, then compare all explicit branching/leaf
// paths with the compact suffix tree. This checks topology and every edge label.
void verify_tree(const string &s, const suffix_tree &tree) {
    struct trie_node {
        map<int, int> next;
        vector<int> path;
        int suffix = -1;
    };
    vector<int> encoded;
    for (unsigned char c : s) encoded.push_back(int(c) + 1);
    encoded.push_back(0);
    assert(tree.text == encoded);
    vector<trie_node> trie(1);
    for (int i = 0; i < static_cast<int>(encoded.size()); ++i) {
        int vertex = 0;
        for (int j = i; j < static_cast<int>(encoded.size()); ++j) {
            auto it = trie[vertex].next.find(encoded[j]);
            if (it == trie[vertex].next.end()) {
                int child = static_cast<int>(trie.size());
                vector<int> path = trie[vertex].path;
                path.push_back(encoded[j]);
                trie[vertex].next[encoded[j]] = child;
                trie.push_back({{}, path, -1});
                vertex = child;
            } else vertex = it->second;
        }
        trie[vertex].suffix = i;
    }
    map<vector<int>, int> expected, actual;
    for (size_t i = 0; i < trie.size(); ++i) {
        if (i == 0 || trie[i].next.size() != 1) expected[trie[i].path] = trie[i].suffix;
    }
    vector<vector<int>> paths(tree.nodes.size());
    vector<int> order{0};
    vector<int> counts(tree.nodes.size());
    for (size_t i = 0; i < order.size(); ++i) {
        int vertex = order[i];
        const auto &node = tree.nodes[vertex];
        assert(size_t(node.depth) == paths[vertex].size());
        assert(actual.emplace(paths[vertex], node.suffix_start).second);
        for (auto [symbol, child] : node.next) {
            const auto &edge = tree.nodes[child];
            assert(edge.parent == vertex);
            assert(0 <= edge.left && edge.left < edge.right && size_t(edge.right) <= encoded.size());
            assert(encoded[edge.left] == symbol);
            paths[child] = paths[vertex];
            paths[child].insert(paths[child].end(), encoded.begin() + edge.left, encoded.begin() + edge.right);
            order.push_back(child);
        }
    }
    assert(order.size() == tree.nodes.size());
    assert(actual == expected);
    for (auto it = order.rbegin(); it != order.rend(); ++it) {
        int vertex = *it;
        const auto &node = tree.nodes[vertex];
        if (node.next.empty()) counts[vertex] = 1;
        else if (vertex != 0) {
            vector<int> suffix(paths[vertex].begin() + 1, paths[vertex].end());
            assert(paths[node.suffix_link] == suffix);
        }
        assert(counts[vertex] == node.leaf_count);
        if (node.parent != -1) counts[node.parent] += counts[vertex];
    }
    assert(tree.nodes[0].leaf_count == static_cast<int>(s.size()) + 1);
    assert(tree.nodes.size() <= 2 * encoded.size());
    set<string> substrings;
    for (size_t i = 0; i < s.size(); ++i) {
        for (size_t length = 1; i + length <= s.size(); ++length) substrings.insert(s.substr(i, length));
    }
    assert(tree.distinct_substrings() == static_cast<int64_t>(substrings.size()));
}

void verify_pattern(const string &s, const suffix_tree &tree, const string &pattern) {
    auto expected = slow_occurrences(s, pattern);
    assert(tree.contains(pattern) == !expected.empty());
    assert(tree.count(pattern) == static_cast<int>(expected.size()));
    assert(tree.occurrences(pattern) == expected);
}

void test_small(mt19937_64 &rng) {
    vector<string> cases = {"", "a", "aaaaaa", "abababab", "banana", "mississippi",
                            "abcabxabcd", "abcdefg", "$#@$#", string("a\0b\0a", 5)};
    // Exhaustive binary strings; reuse the same object to check build resets.
    for (int length = 0; length <= 8; ++length) {
        for (int mask = 0; mask < (1 << length); ++mask) {
            string s(length, 'a');
            for (int i = 0; i < length; ++i) s[i] += (mask >> i) & 1;
            cases.push_back(s);
        }
    }
    for (int iteration = 0; iteration < 600; ++iteration) {
        string s(rng() % 35, '\0');
        int alphabet = iteration % 3 == 0 ? 256 : 4;
        for (char &c : s) c = static_cast<char>(rng() % alphabet);
        cases.push_back(s);
    }
    suffix_tree tree;
    for (const string &s : cases) {
        tree.build(s);
        verify_tree(s, tree);
        verify_pattern(s, tree, "");
        verify_pattern(s, tree, s + 'x');
        for (size_t i = 0; i < s.size(); ++i) {
            for (size_t length = 1; i + length <= s.size(); ++length) {
                verify_pattern(s, tree, s.substr(i, length));
            }
        }
        for (int i = 0; i < 10; ++i) {
            string pattern(rng() % 8, '\0');
            for (char &c : pattern) c = static_cast<char>(rng() % 256);
            verify_pattern(s, tree, pattern);
        }
    }
    string all_bytes;
    for (int c = 0; c < 256; ++c) all_bytes += static_cast<char>(c);
    suffix_tree bytes(all_bytes + all_bytes);
    for (int c = 0; c < 256; ++c) assert(bytes.count(string(1, static_cast<char>(c))) == 2);
    verify_pattern(all_bytes + all_bytes, bytes, all_bytes);
}

void test_large(mt19937_64 &rng) {
    const int n = 100000;
    string repeated(n, 'a');
    suffix_tree tree(repeated);
    assert(tree.count(string(1000, 'a')) == n - 999);
    assert(tree.distinct_substrings() == n);
    assert(!tree.contains(repeated + 'a'));
    verify_pattern(repeated, tree, "aaa");
    string periodic(n, 'a');
    for (int i = 1; i < n; i += 2) periodic[i] = 'b';
    tree.build(periodic);
    assert(tree.count("abab") == n / 2 - 1);
    assert(tree.distinct_substrings() == 2 * n - 1);
    string random(n, '\0');
    for (char &c : random) c = static_cast<char>(rng() % 256);
    tree.build(random);
    assert(tree.nodes[0].leaf_count == n + 1);
    for (int i = 0; i < 30; ++i) {
        size_t start = rng() % (n - 100);
        verify_pattern(random, tree, random.substr(start, 1 + rng() % 100));
    }
}

signed main() {
    mt19937_64 rng(123591);
    test_small(rng);
    test_large(rng);
    cout << "suffix tree tests passed\n";
}
