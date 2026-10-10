struct fenwick {
    vector<long> f;

    void init(int n) {
        f.assign(n + 1, 0);
    }

    long sum(int r) { // exclusive
        r--;
        long result = 0;
        for (; r >= 0; r = (r & (r + 1)) - 1)
            result += f[r];
        return result;
    }

    void inc(int i, long x) {
        for (; i < f.size(); i = (i | (i + 1)))
            f[i] += x;
    }
};
