vector<node> nodes;

int newnode() {
    nodes.emplace_back();
    return (int)nodes.size() - 1;
}

int init(int l, int r) {
    int c = newnode();
    if (r > l + 1) {
        int m = (l + r) / 2;
        nodes[c].left = init(l, m);
        nodes[c].right = init(m, r);
    }
    return c;
}

int add(int x, int l, int r, int i, int d) {
    int c = newnode();
    nodes[c] = nodes[x];
    nodes[c].sum += d;
    if (r > l + 1) {
        int m = (l + r) / 2;
        if (i < m) {
            nodes[c].left = add(nodes[x].left, l, m, i, d);
        } else {
            nodes[c].right = add(nodes[x].right, m, r, i, d);
        }
    }
    return c;
}

int findright(int x, int l, int r, int p) {
    if (nodes[x].sum == 0) return INT_MAX;
    if (p >= r) return INT_MAX;
    if (r == l + 1) return l;
    int m = (l + r) / 2;
    int res = findright(nodes[x].left, l, m, p);
    if (res == INT_MAX) {
        res = findright(nodes[x].right, m, r, p);
    }
    return res;
}
int findleft(int x, int l, int r, int p) {
    if (nodes[x].sum == 0) return INT_MIN;
    if (p < l) return INT_MIN;
    if (r == l + 1) return l;
    int m = (l + r) / 2;
    int res = findleft(nodes[x].right, m, r, p);
    if (res == INT_MIN) {
        res = findleft(nodes[x].left, l, m, p);
    }
    return res;
}
