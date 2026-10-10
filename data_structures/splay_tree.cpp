struct splay_tree {

    struct Node;

    vector<Node> nodes;

    struct Node {
        int id, left, right, parent, size;
        int key, val;
    };

    int size(int x) {
        if (x == -1) return 0;
        return nodes[x].size;
    }

    void recalc(int x) {
        if (x == -1) return;
        nodes[x].size = 1;
        int k = 0;
        nodes[x].val = INT_MIN;
        if (nodes[x].left >= 0) {
            nodes[nodes[x].left].parent = nodes[x].id;
            nodes[x].size += nodes[nodes[x].left].size;
            k += nodes[nodes[x].left].size;
            nodes[x].val = ::max(nodes[x].val, nodes[nodes[x].left].val);
        }
        nodes[x].val = ::max(nodes[x].val, nodes[x].key - k);
        k++;
        if (nodes[x].right >= 0) {
            nodes[nodes[x].right].parent = nodes[x].id;
            nodes[x].size += nodes[nodes[x].right].size;
            nodes[x].val = ::max(nodes[x].val, nodes[nodes[x].right].val - k);
        }
    }

    void init(int n) {
        nodes.resize(n);
        for (int i = 0; i < n; i++) {
            nodes[i].id = i;
            nodes[i].left = nodes[i].right = nodes[i].parent = -1;
            recalc(i);
        }
    }

    void rotate(int x) {
        int p = nodes[x].parent;
        int g = nodes[p].parent;
        if (nodes[p].left == x) {
            nodes[p].left = nodes[x].right;
            nodes[x].right = p;
        } else {
            nodes[p].right = nodes[x].left;
            nodes[x].left = p;
        }
        recalc(p);
        recalc(x);
        if (g >= 0) {
            if (nodes[g].left == p) nodes[g].left = x; else nodes[g].right = x;
            recalc(g);
        } else {
            nodes[x].parent = -1;
        }
    }

    void splay(int x) {
        while (nodes[x].parent >= 0) {
            int p = nodes[x].parent;
            int g = nodes[p].parent;
            if (g == -1) {
                rotate(x);
                return;
            }
            if ((x == nodes[p].right && p == nodes[g].right) ||
                (x == nodes[p].left && p == nodes[g].left)) {
                rotate(p);
                rotate(x);
            } else {
                rotate(x);
                rotate(x);
            }
        }
    }

    int max(int x) {
        while (nodes[x].right >= 0) {
            x = nodes[x].right;
        }
        splay(x);
        return x;
    }

    int insert_simple(int root, int x) {
        if (root == -1) {
            return x;
        } else {
            if (nodes[x].key > nodes[root].key) {
                nodes[root].right = insert_simple(nodes[root].right, x);
                recalc(root);
            } else {
                nodes[root].left = insert_simple(nodes[root].left, x);
                recalc(root);
            }
            return root;
        }
    }

    int insert(int root, int x) {
        insert_simple(root, x);
        splay(x);
        return x;
    }

    int remove(int x) {
        splay(x);
        int l = nodes[x].left;
        int r = nodes[x].right;
        nodes[x].left = -1;
        nodes[x].right = -1;
        recalc(x);
        if (l >= 0) nodes[l].parent = -1;
        if (r >= 0) nodes[r].parent = -1;
        return merge(l, r);
    }

    int merge(int x, int y) {
        if (x == -1) {
            return y;
        }
        x = max(x);
        nodes[x].right = y;
        recalc(x);
        return x;
    }
};
