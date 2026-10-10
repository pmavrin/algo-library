struct sufmas {

    void count_sort(vector<int> &p, vector<int> &c) {
        int n = p.size();
        vector<int> cnt(n);
        for (auto x: c) {
            cnt[x]++;
        }
        vector<int> p_new(n);
        vector<int> pos(n);
        pos[0] = 0;
        for (int i = 1; i < n; i++) {
            pos[i] = pos[i - 1] + cnt[i - 1];
        }
        for (auto x: p) {
            int i = c[x];
            p_new[pos[i]] = x;
            pos[i]++;
        }
        p = p_new;
    }

    vector<int> p;
    vector<int> c;
    vector<int> lcp;
    vector<vector<int>> st;
    const int MAX = 25;
    const char SPECIAL = '#';

    sufmas(string s) {
        int n = s.size();
        p.resize(n);
        c.resize(n);
        {
            // k = 0
            vector<pair<char, int>> a(n);
            for (int i = 0; i < n; i++) a[i] = {s[i], i};
            sort(a.begin(), a.end());

            for (int i = 0; i < n; i++) p[i] = a[i].second;
            c[p[0]] = 0;
            for (int i = 1; i < n; i++) {
                if (a[i].first == a[i - 1].first && a[i].first != SPECIAL) {
                    c[p[i]] = c[p[i - 1]];
                } else {
                    c[p[i]] = c[p[i - 1]] + 1;
                }
            }
        }

        int k = 0;
        while ((1 << k) < n) {
            // k -> k + 1

            for (int i = 0; i < n; i++) {
                p[i] = (p[i] - (1 << k) + n) % n;
            }

            count_sort(p, c);

            vector<int> c_new(n);
            c_new[p[0]] = 0;
            for (int i = 1; i < n; i++) {
                pair<int, int> prev = {c[p[i - 1]], c[(p[i - 1] + (1 << k)) % n]};
                pair<int, int> now = {c[p[i]], c[(p[i] + (1 << k)) % n]};
                if (now == prev) {
                    c_new[p[i]] = c_new[p[i - 1]];
                } else {
                    c_new[p[i]] = c_new[p[i - 1]] + 1;
                }
            }
            c = c_new;
            k++;
        }

        lcp.resize(n);
        k = 0;
        for (int i = 0; i < n - 1; i++) {
            int pi = c[i];
            if (pi > 0) {
                int j = p[pi - 1];
                // lcp[i] = lcp(s[i..], s[j..])
                while (s[i + k] == s[j + k] && s[i + k] != SPECIAL) k++;
            }
            lcp[pi] = k;
            k = max(k - 1, 0);
        }

        st.assign(MAX, vector<int>(n));
        st[0] = lcp;
        for (int k = 1; k < MAX; k++) {
            for (int i = 0; i + (1 << k) <= n; i++) {
                st[k][i] = min(st[k - 1][i], st[k - 1][i + (1 << (k - 1))]);
            }
        }
    }

    int get_lcp(int i, int j) {
        i = c[i];
        j = c[j];
        if (i > j) swap(i, j);
        i++,j++;
        int d = (j - i);
        int k = 0;
        while ((1 << (k + 1)) <= d) k++;
        return min(st[k][i], st[k][j - (1 << k)]);
    }

};
