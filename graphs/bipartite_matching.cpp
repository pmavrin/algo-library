// Hopcroft-Karp maximum matching in a bipartite graph. O(E * sqrt(V)).

struct bipartite_matching {
    int left_size;
    int right_size;
    vector<vector<int>> graph;
    vector<int> match_left;
    vector<int> match_right;
    vector<int> distance;
    vector<int> pointer;
    int shortest_augmenting_path = -1;

    bipartite_matching(int left_size, int right_size)
        : left_size(left_size),
          right_size(right_size),
          graph(left_size),
          match_left(left_size, -1),
          match_right(right_size, -1),
          distance(left_size),
          pointer(left_size) {
        assert(left_size >= 0 && right_size >= 0);
    }

    void add_edge(int left, int right) {
        assert(0 <= left && left < left_size && 0 <= right && right < right_size);
        graph[left].push_back(right);
    }

    int maximum_matching() {
        int result = 0;
        for (int right : match_left) result += right != -1;
        while (build_layers()) {
            fill(pointer.begin(), pointer.end(), 0);
            for (int left = 0; left < left_size; ++left) {
                if (match_left[left] == -1 && augment(left)) ++result;
            }
        }
        return result;
    }

    pair<vector<int>, vector<int>> minimum_vertex_cover() const {
        vector<char> visited_left(left_size, false);
        vector<char> visited_right(right_size, false);
        vector<int> queue;
        for (int left = 0; left < left_size; ++left) {
            if (match_left[left] == -1) {
                visited_left[left] = true;
                queue.push_back(left);
            }
        }
        for (int head = 0; head < static_cast<int>(queue.size()); ++head) {
            int left = queue[head];
            for (int right : graph[left]) {
                if (match_left[left] == right || visited_right[right]) continue;
                visited_right[right] = true;
                int next_left = match_right[right];
                if (next_left != -1 && !visited_left[next_left]) {
                    visited_left[next_left] = true;
                    queue.push_back(next_left);
                }
            }
        }

        vector<int> left_cover;
        vector<int> right_cover;
        for (int left = 0; left < left_size; ++left) {
            if (!visited_left[left]) left_cover.push_back(left);
        }
        for (int right = 0; right < right_size; ++right) {
            if (visited_right[right]) right_cover.push_back(right);
        }
        return {left_cover, right_cover};
    }

private:
    bool build_layers() {
        fill(distance.begin(), distance.end(), -1);
        shortest_augmenting_path = -1;
        vector<int> queue;
        for (int left = 0; left < left_size; ++left) {
            if (match_left[left] == -1) {
                distance[left] = 0;
                queue.push_back(left);
            }
        }
        for (int head = 0; head < static_cast<int>(queue.size()); ++head) {
            int left = queue[head];
            if (shortest_augmenting_path != -1 &&
                distance[left] + 1 > shortest_augmenting_path) continue;
            for (int right : graph[left]) {
                int next_left = match_right[right];
                if (next_left == -1) {
                    shortest_augmenting_path = distance[left] + 1;
                } else if (distance[next_left] == -1) {
                    distance[next_left] = distance[left] + 1;
                    queue.push_back(next_left);
                }
            }
        }
        return shortest_augmenting_path != -1;
    }

    bool augment(int left) {
        for (int &edge_index = pointer[left];
             edge_index < static_cast<int>(graph[left].size());
             ++edge_index) {
            int right = graph[left][edge_index];
            int next_left = match_right[right];
            bool finishes_path = next_left == -1 &&
                distance[left] + 1 == shortest_augmenting_path;
            bool continues_path = next_left != -1 &&
                distance[next_left] == distance[left] + 1 && augment(next_left);
            if (finishes_path || continues_path) {
                match_left[left] = right;
                match_right[right] = left;
                return true;
            }
        }
        distance[left] = -1;
        return false;
    }
};
