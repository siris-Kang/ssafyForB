#include <iostream>
#include <algorithm>
#include <queue>
#include <vector>
#define debug 
using namespace std;
 
const int INF = 1e9;
 
int N, K;
 
struct Edge {
    int v, w;
};
 
struct State {
    int node, dist;
 
    bool operator>(const State& other) const {
        return dist > other.dist;
    }
};
 
vector<vector<Edge>> adj;
vector<int> distCoffee;
vector<int> distBakery;
 
void init(int n, int k, int sBuilding[], int eBuilding[], int mDistance[]) {
    N = n; K = k;
 
    adj.assign(N, {});
    distCoffee.resize(N);
    distBakery.resize(N);
 
    for (int i = 0; i < K; i++) {
        int u = sBuilding[i];
        int v = eBuilding[i];
        int w = mDistance[i];
 
        adj[u].push_back({ v, w });
        adj[v].push_back({ u, w });
    }
}
 
void add(int sBuilding, int eBuilding, int mDistance) {
    adj[sBuilding].push_back({ eBuilding, mDistance });
    adj[eBuilding].push_back({ sBuilding, mDistance });
}
 
void dijkstra(int startCount, int startNodes[],
    vector<int>& dist, int R) {
 
    priority_queue<State, vector<State>, greater<State>> pq;
 
    // dist 초기화
    fill(dist.begin(), dist.end(), INF);
 
    for (int i = 0; i < startCount; i++) {
        int s = startNodes[i];
        dist[s] = 0;
        pq.push({ s, 0 });
    }
 
    while (!pq.empty()) {
        State cur = pq.top(); pq.pop();
 
        if (cur.dist > R) break;                 
        if (cur.dist > dist[cur.node]) continue;
 
        for (auto& nxt : adj[cur.node]) {
            int nd = cur.dist + nxt.w;
 
            if (nd > R) continue;
            if (dist[nxt.v] <= nd) continue;
 
            dist[nxt.v] = nd;
            pq.push({ nxt.v, nd });
        }
    }
}
int calculate(int M, int mCoffee[],int P, int mBakery[],int R) {
 
    dijkstra(M, mCoffee, distCoffee, R);
    dijkstra(P, mBakery, distBakery, R);
 
    int ans = INF;
 
    for (int i = 0; i < N; i++) {
        if (distCoffee[i] == 0 || distBakery[i] == 0) continue;
        if (distCoffee[i] <= R && distBakery[i] <= R) {
            ans = min(ans, distCoffee[i] + distBakery[i]);
        }
    }
    debug("다익스트라 결과\n");
    for (int i = 0; i < N; i++) {
        debug("%d ", distCoffee[i]);
    }debug("\n");
 
    for (int i = 0; i < N; i++) {
        debug("%d ", distBakery[i]);
    }debug("\n");
 
    debug("ans : %d", ans);
    return (ans == INF ? -1 : ans);
}
