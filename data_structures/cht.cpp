// Minimum of k*x + b at integer x. Insert slopes in non-increasing order.
// add_line: amortized O(1). query: O(log n), arbitrary x.
// query_monotone: amortized O(1), non-decreasing x across these calls.
// Both query methods can be mixed; only query_monotone calls must be ordered.
// Coefficients and x are int64_t, answers and breakpoints use __int128_t.

struct cht {
    struct line {
        int64_t k, b;
        __int128_t start; // first integer x where this line is optimal

        __int128_t eval(int64_t x) const {
            return __int128_t(k) * x + b;
        }
    };

    bool empty() const { return lines.empty(); }

    void clear() {
        lines.clear();
        head = 0;
        last_x.reset();
    }

    void add_line(int64_t k, int64_t b) {
        assert(empty() || k <= lines.back().k);
        if (!empty() && k == lines.back().k) {
            if (b >= lines.back().b) return;
            lines.pop_back();
        }
        __int128_t start = negative_infinity;
        while (!empty()) {
            // ceil((b - previous.b) / (previous.k - k)); denominator > 0.
            // C++ division rounds toward zero, including negative numerators.
            __int128_t numerator = __int128_t(b) - lines.back().b;
            __int128_t denominator = __int128_t(lines.back().k) - k;
            start = numerator / denominator + (numerator % denominator > 0);
            if (start > lines.back().start) break;
            lines.pop_back();
        }
        if (empty()) start = negative_infinity;
        lines.push_back({k, b, start});
        head = min(head, lines.size() - 1);
    }

    __int128_t query(int64_t x) const {
        assert(!empty());
        size_t left = 0, right = lines.size();
        while (right - left > 1) {
            size_t middle = left + (right - left) / 2;
            if (lines[middle].start <= x) left = middle;
            else right = middle;
        }
        return lines[left].eval(x);
    }

    __int128_t query_monotone(int64_t x) {
        assert(!empty());
        assert(!last_x || *last_x <= x);
        last_x = x;
        while (head + 1 < lines.size() && lines[head + 1].start <= x) ++head;
        return lines[head].eval(x);
    }

private:
    static constexpr __int128_t negative_infinity = -(__int128_t(1) << 126);
    vector<line> lines;
    size_t head = 0;
    optional<int64_t> last_x;
};
