// Explicit suffix tree, built by Ukkonen's active-point algorithm.
// Algorithm references:
// https://cp-algorithms.com/string/suffix-tree-ukkonen.html
// https://github.com/kth-competitive-programming/kactl/blob/main/content/strings/SuffixTree.h
// This implementation uses dynamic nodes and map transitions, with no recursion.
// Build: O(n log(sigma+1)) time, O(n) memory; sigma <= 257 for byte strings.
// The public interface builds the whole string at once (no append operation).
// Bytes are encoded as unsigned_char + 1; a unique terminal 0 is added internally.
// Indices/lengths count bytes, not Unicode characters. Root is node 0.

struct suffix_tree {
    struct node {
        // Incoming edge is text[left..right), including the terminal if present.
        int left, right, parent;
        int suffix_link = 0; // meaningful for internal nodes; root links to itself
        map<int, int> next;
        int depth = 0; // length of the entire root-to-node path
        int suffix_start = -1; // leaves only; includes the empty suffix at n
        int leaf_count = 0;
    };

    vector<int> text;
    vector<node> nodes;

    explicit suffix_tree(const string &s = "") { build(s); }

    void build(const string &s) {
        text.clear();
        text.reserve(s.size() + 1);
        for (unsigned char c : s) text.push_back(int(c) + 1);
        text.push_back(0);
        int total = static_cast<int>(text.size());
        nodes.clear();
        nodes.reserve(2 * text.size());
        make_node(0, 0, -1);

        int active = 0, edge = 0, length = 0, pending = 0;
        for (int position = 0; position < total; ++position) {
            ++pending;
            int previous_split = -1;
            while (pending > 0) {
                if (length == 0) edge = position;
                auto it = nodes[active].next.find(text[edge]);
                if (it == nodes[active].next.end()) {
                    int leaf = make_node(position, total, active);
                    nodes[active].next[text[edge]] = leaf;
                    if (previous_split != -1) nodes[previous_split].suffix_link = active;
                    previous_split = -1;
                } else {
                    int child = it->second;
                    // Leaves end at the current phase, not the final string end.
                    int edge_length = min(nodes[child].right, position + 1) - nodes[child].left;
                    if (length >= edge_length) {
                        edge += edge_length;
                        length -= edge_length;
                        active = child;
                        continue;
                    }
                    if (text[nodes[child].left + length] == text[position]) {
                        if (previous_split != -1) nodes[previous_split].suffix_link = active;
                        ++length;
                        break;
                    }
                    int middle = make_node(nodes[child].left, nodes[child].left + length, active);
                    nodes[active].next[text[edge]] = middle;
                    int leaf = make_node(position, total, middle);
                    nodes[middle].next[text[position]] = leaf;
                    nodes[child].left += length;
                    nodes[child].parent = middle;
                    nodes[middle].next[text[nodes[child].left]] = child;
                    if (previous_split != -1) nodes[previous_split].suffix_link = middle;
                    previous_split = middle;
                }
                --pending;
                if (active == 0 && length > 0) {
                    --length;
                    edge = position - pending + 1;
                } else if (active != 0) {
                    active = nodes[active].suffix_link;
                }
            }
        }

        // Parent-before-child order, then aggregate leaf counts bottom-up.
        vector<int> order{0};
        for (size_t i = 0; i < order.size(); ++i) {
            int vertex = order[i];
            for (auto [symbol, child] : nodes[vertex].next) {
                nodes[child].depth = nodes[vertex].depth + nodes[child].right - nodes[child].left;
                order.push_back(child);
            }
        }
        for (auto it = order.rbegin(); it != order.rend(); ++it) {
            int vertex = *it;
            if (nodes[vertex].next.empty()) {
                nodes[vertex].suffix_start = total - nodes[vertex].depth;
                nodes[vertex].leaf_count = 1;
            }
            if (nodes[vertex].parent != -1) {
                nodes[nodes[vertex].parent].leaf_count += nodes[vertex].leaf_count;
            }
        }
    }

    // Returns -1 if absent, otherwise the node whose incoming edge contains
    // the end of pattern. This also works when the pattern ends mid-edge.
    // Empty pattern maps to the root. O(pattern.size() * log(sigma+1)).
    int find_node(const string &pattern) const {
        int vertex = 0;
        size_t matched = 0;
        while (matched < pattern.size()) {
            int symbol = int(static_cast<unsigned char>(pattern[matched])) + 1;
            auto it = nodes[vertex].next.find(symbol);
            if (it == nodes[vertex].next.end()) return -1;
            vertex = it->second;
            for (int position = nodes[vertex].left;
                 position < nodes[vertex].right && matched < pattern.size(); ++position, ++matched) {
                if (text[position] != int(static_cast<unsigned char>(pattern[matched])) + 1) return -1;
            }
        }
        return vertex;
    }

    bool contains(const string &pattern) const { return find_node(pattern) != -1; }

    // Overlapping occurrences are counted; empty pattern occurs n+1 times.
    int count(const string &pattern) const {
        int vertex = find_node(pattern);
        return vertex == -1 ? 0 : nodes[vertex].leaf_count;
    }

    // Sorted start positions. O(|pattern| log(sigma+1) + k log(k+1)), k = count.
    vector<int> occurrences(const string &pattern) const {
        int vertex = find_node(pattern);
        if (vertex == -1) return {};
        vector<int> result, stack{vertex};
        while (!stack.empty()) {
            int current = stack.back();
            stack.pop_back();
            if (nodes[current].suffix_start != -1) result.push_back(nodes[current].suffix_start);
            for (auto [symbol, child] : nodes[current].next) stack.push_back(child);
        }
        sort(result.begin(), result.end());
        return result;
    }

    // Counts nonempty distinct substrings of the original string, in O(n).
    int64_t distinct_substrings() const {
        int n = static_cast<int>(text.size()) - 1;
        int64_t result = 0;
        for (size_t i = 1; i < nodes.size(); ++i) {
            result += min(nodes[i].right, n) - nodes[i].left;
        }
        return result;
    }

private:
    int make_node(int left, int right, int parent) {
        nodes.push_back({left, right, parent, 0, {}, 0, -1, 0});
        return static_cast<int>(nodes.size()) - 1;
    }
};
