#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

const int MAX_N = 10005;
const int INF = 1e9;

struct Edge {
    int to, dist;
};

struct State {
    int d, type, u;
    bool operator>(const State& o) const {
        return d > o.d;
    }
};

int N;
vector<Edge> graph[MAX_N];
int distCoffee[MAX_N];
int distBakery[MAX_N];
bool isShop[MAX_N];
vector<int> modified;

void init(int N_, int K, int sBuilding[], int eBuilding[], int mDistance[]) {
    N = N_;
    for (int i = 0; i < N; i++) {
        graph[i].clear();
        distCoffee[i] = INF;
        distBakery[i] = INF;
        isShop[i] = false;
    }
    modified.clear();

    for (int i = 0; i < K; i++) {
        graph[sBuilding[i]].push_back({ eBuilding[i], mDistance[i] });
        graph[eBuilding[i]].push_back({ sBuilding[i], mDistance[i] });
    }
}

void add(int sBuilding, int eBuilding, int mDistance) {
    graph[sBuilding].push_back({ eBuilding, mDistance });
    graph[eBuilding].push_back({ sBuilding, mDistance });
}

int calculate(int M, int mCoffee[], int P, int mBakery[], int R) {
    for (int idx : modified) {
        distCoffee[idx] = INF;
        distBakery[idx] = INF;
        isShop[idx] = false;
    }
    modified.clear();

    priority_queue<State, vector<State>, greater<State>> pq;

    for (int i = 0; i < M; i++) {
        int u = mCoffee[i];
        distCoffee[u] = 0;
        isShop[u] = true;
        pq.push({ 0, 0, u });
        modified.push_back(u);
    }

    for (int i = 0; i < P; i++) {
        int u = mBakery[i];
        distBakery[u] = 0;
        isShop[u] = true;
        pq.push({ 0, 1, u });
        modified.push_back(u);
    }

    while (!pq.empty()) {
        State cur = pq.top();
        pq.pop();

        if (cur.type == 0 && cur.d > distCoffee[cur.u]) continue;
        if (cur.type == 1 && cur.d > distBakery[cur.u]) continue;

        for (auto& edge : graph[cur.u]) {
            int v = edge.to;
            int newDist = cur.d + edge.dist;

            if (newDist > R) continue;

            if (cur.type == 0) {
                if (newDist < distCoffee[v]) {
                    if (distCoffee[v] == INF && distBakery[v] == INF) modified.push_back(v);
                    distCoffee[v] = newDist;
                    pq.push({ newDist, 0, v });
                }
            }
            else {
                if (newDist < distBakery[v]) {
                    if (distCoffee[v] == INF && distBakery[v] == INF) modified.push_back(v);
                    distBakery[v] = newDist;
                    pq.push({ newDist, 1, v });
                }
            }
        }
    }

    int minSum = INF;
    for (int i : modified) {
        if (isShop[i]) continue;
        if (distCoffee[i] <= R && distBakery[i] <= R) {
            minSum = min(minSum, distCoffee[i] + distBakery[i]);
        }
    }

    return (minSum == INF) ? -1 : minSum;
}