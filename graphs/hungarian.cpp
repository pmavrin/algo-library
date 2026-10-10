// Minimum-cost assignment for n rows and m columns, n <= m. O(n^2 * m).
// Returns (cost, assignment), where assignment[row] is its selected column.

pair<int64_t, vector<int>> hungarian(const vector<vector<int64_t>> &cost) {
    int rows = static_cast<int>(cost.size());
    if (rows == 0) return {0, {}};
    int columns = static_cast<int>(cost[0].size());
    assert(rows <= columns);
    for (const auto &row : cost) assert(static_cast<int>(row.size()) == columns);

    const int64_t infinity = numeric_limits<int64_t>::max() / 4;
    vector<int64_t> row_potential(rows + 1);
    vector<int64_t> column_potential(columns + 1);
    vector<int> matching(columns + 1);
    vector<int> previous(columns + 1);

    for (int row = 1; row <= rows; ++row) {
        matching[0] = row;
        int column = 0;
        vector<int64_t> minimum(columns + 1, infinity);
        vector<char> used(columns + 1, false);
        do {
            used[column] = true;
            int current_row = matching[column];
            int next_column = 0;
            int64_t delta = infinity;
            for (int candidate = 1; candidate <= columns; ++candidate) {
                if (used[candidate]) continue;
                int64_t current = static_cast<int64_t>(cost[current_row - 1][candidate - 1])
                    - row_potential[current_row] - column_potential[candidate];
                if (current < minimum[candidate]) {
                    minimum[candidate] = current;
                    previous[candidate] = column;
                }
                if (minimum[candidate] < delta) {
                    delta = minimum[candidate];
                    next_column = candidate;
                }
            }
            for (int candidate = 0; candidate <= columns; ++candidate) {
                if (used[candidate]) {
                    row_potential[matching[candidate]] += delta;
                    column_potential[candidate] -= delta;
                } else {
                    minimum[candidate] -= delta;
                }
            }
            column = next_column;
        } while (matching[column] != 0);

        do {
            int next_column = previous[column];
            matching[column] = matching[next_column];
            column = next_column;
        } while (column != 0);
    }

    vector<int> assignment(rows, -1);
    for (int column = 1; column <= columns; ++column) {
        if (matching[column] != 0) assignment[matching[column] - 1] = column - 1;
    }
    int64_t result = 0;
    for (int row = 0; row < rows; ++row) result += cost[row][assignment[row]];
    return {result, assignment};
}
