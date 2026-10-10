#include <bits/stdc++.h>
#include <immintrin.h>

using namespace std;

int main() {

    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    cout << rng() << "\n";
    vector<int> a;
    shuffle(a.begin(), a.end(), rng);

    return 0;
}
