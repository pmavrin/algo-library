struct Dsu {

    vector<int> p;

    Dsu(int n) {
        p.resize(n);
        for (int i = 0; i < n; i++) p[i] = i;
    }

    int get(int x) {
        if (p[x] != x) p[x] = get(p[x]);
        return p[x];
    }

    bool unite(int x, int y) {
        x = get(x);
        y = get(y);
        if (x == y) return false;
        p[x] = y;
        return true;
    }
};
