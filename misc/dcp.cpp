struct segment {
    int l, r, v;
};

vector<int> y;

void add(int x) {
}

void remove() {
}

void g(int l, int r, vector<segment> ss) {
    int m = (l + r) / 2;
    int maxSave = mmax;

    int added = 0;
    vector<segment> sl;
    vector<segment> sr;
    for (auto s : ss) {
        if (s.l <= l && s.r >= r) {
            added++;
            add(s.v);
        } else {
            if (s.l < m) sl.push_back(s);
            if (s.r > m) sr.push_back(s);
        }
    }

    if (r == l + 1) {
        y[l] = mmax;
    } else {
        g(l, m, sl);
        g(m, r, sr);
    }

    mmax = maxSave;
    for (int i = 0; i < added; i++) {
        remove();
    }
}