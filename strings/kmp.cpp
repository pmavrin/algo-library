vector<int> kmp(string s) {
    int n = s.size();
    vector<int> p(n + 1);
    p[0] = -1;
    for (int i = 1; i <= n; i++) {
        int k = p[i - 1];
        while (k >= 0 && s[k] != s[i - 1]) k = p[k];
        p[i] = k + 1;
    }
    return p;
}
