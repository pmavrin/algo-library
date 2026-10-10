#include <bits/stdc++.h>

#define long long long int
using namespace std;

// @author: pashka

int n, m;
vector<pair<int, int>> res;

struct point {
    long x, y;
    int id;

    long operator%(point o) {
        return x * o.y - y * o.x;
    }

    point operator-(point o) {
        return {x - o.x, y - o.y};
    }

    int type() {
        return id < n;
    }
};

vector<point> p;

void solve(point a, point b, point c) {
    for (point pp : p) {
        if (((b - a) % (pp - a) > 0) &&
            ((c - b) % (pp - b) > 0) &&
            ((a - c) % (pp - c) > 0) &&
            pp.type() == a.type()) {
            res.push_back({a.id, pp.id});
            solve(b, pp, a);
            solve(c, a, pp);
            solve(pp, b, c);
            return;
        }
    }
    for (point pp : p) {
        if (((b - a) % (pp - a) > 0) &&
            ((c - b) % (pp - b) > 0) &&
            ((a - c) % (pp - c) > 0)) {
            res.push_back({b.id, pp.id});
        }
    }
}

int main() {
    ios::sync_with_stdio(false);

    cin >> n >> m;
    p.resize(n + m);
    for (int i = 0; i < n + m; i++) {
        cin >> p[i].x >> p[i].y;
        p[i].id = i;
    }

    int o = 0;
    for (int i = 1; i < n + m; i++) {
        if (p[i].x < p[o].x) o = i;
    }
    swap(p[0], p[o]);
    for (int i = 1; i < n + m; i++) {
        p[i].x -= p[0].x;
        p[i].y -= p[0].y;
    }
    p[0].x = 0;
    p[0].y = 0;

    sort(p.begin() + 1, p.end(),
         [](point &a, point &b) -> bool {
             return (a % b) > 0;
         });


    vector<point> st;
    st.push_back(p[0]);
    for (int i = 1; i < n + m; i++) {
        while (st.size() >= 2 &&
               (st[st.size() - 1] - st[st.size() - 2]) %
               (p[i] - st[st.size() - 1]) < 0) {
            st.pop_back();
        }
        st.push_back(p[i]);
    }

//    for (auto pp : st) {
//        cout << pp.x << " " << pp.y << "\n";
//    }
//    cout << "\n";

    int x = 0;
    while (x < st.size() && st[x].type() == st[0].type()) x++;

    if (x == st.size()) {
        for (int i = 0; i < st.size() - 1; i++) {
            res.push_back({st[i].id, st[i + 1].id});
        }
        for (int i = 0; i < n + m; i++) {
            if (p[i].type() != st[0].type()) {
                for (int j = 0; j < st.size(); j++) {
                    solve(p[i], st[j], st[(j + 1) % st.size()]);
                }
                break;
            }
        }
    } else {
        vector<point> s2;
        for (int i = x; i < st.size(); i++) s2.push_back(st[i]);
        for (int i = 0; i < x; i++) s2.push_back(st[i]);
        st = s2;
        x = 0;
        while (x < st.size() && st[x].type() == st[0].type()) x++;
        for (int i = x; i < st.size(); i++) {
            if (st[i].type() == st[0].type()) {
                cout << "Impossible\n";
                return 0;
            }
        }
        for (int i = 0; i < st.size() - 1; i++) {
            if (i != x - 1)
                res.push_back({st[i].id, st[i + 1].id});
        }
        for (int i = x; i < st.size() - 1; i++) {
            solve(st[0], st[i], st[i + 1]);
        }
        for (int i = 0; i < x - 1; i++) {
            solve(st[x], st[i], st[i + 1]);
        }
    }
    sort(res.begin(), res.end());
    for (auto e: res) {
        int x = e.first + 1;
        int y = e.second + 1;
        if (x > n) {
            x -= n;
            y -= n;
        }
        cout << x << " " << y << "\n";
    }
    return 0;
}