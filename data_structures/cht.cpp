#include <cstdio>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <cassert>

#define MIN_INT -2147483648
#define MAX_INT 2147483647
#define MIN_LONG -9223372036854775808L
#define MAX_LONG 9223372036854775807L
#define PI 3.141592653589793238462643383279502884L

#define long long long int

using std::vector;
using std::map;
using std::set;
using std::string;
using std::pair;
using std::cin;
using std::cout;
using std::cerr;

// @author: pashka

vector<pair<long, long>> lines;
vector<long> left;

void add_line(long k, long b) {
    if (lines.size() > 0) {
        assert(k <= lines.back().first);
    }
    if (lines.size() > 0 && lines.back().first == k) {
        if (b < lines.back().second) {
            lines.pop_back();
            left.pop_back();
        } else {
            return;
        }
    }
    while (lines.size() > 1) {
        long x = left.back();
        auto line = lines.back();
        if (x * k + b < x * line.first + line.second) {
            lines.pop_back();
            left.pop_back();
        } else {
            break;
        }
    }
    if (lines.size() == 0) {
        left.push_back(MIN_LONG);
    } else {
        // min x : kx+b <= k'x+b'
        //         (b-b') <= (k'-k)x
        long dk = lines.back().first - k;
        long db = b - lines.back().second;
        long x = db / dk;
//        if (x * dk < db) x++;
        left.push_back(x);
    }
    lines.push_back({k, b});
}

long get_min(long x) {
    int l = 0;
    int r = lines.size();
    while (r > l + 1) {
        int m = (l + r) / 2;
        if (x >= left[m]) {
            l = m;
        } else {
            r = m;
        }
    }
    auto line = lines[l];
    return line.first * x + line.second;
}

int main() {
    std::ios::sync_with_stdio(false);

    freopen("input.txt", "r", stdin);

    int n;
    cin >> n;
    vector<long> a(n), b(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }
    for (int i = 0; i < n; i++) {
        cin >> b[i];
    }
    for (int i = n - 1; i >= 0; i--) {
        long q = i == n - 1 ? 0 : get_min(b[i]);
        if (i == 0) {
            cout << std::setprecision(20);
            cout << q << "\n";
        }
        add_line(a[i], q);
    }

    return 0;
}