#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <climits>

using namespace std;

const int MAX_N = 5005;
const int INF = INT_MAX;

struct Cost {
    int totalDist;
    int maxEdge;

    bool operator>(const Cost& other) const {
        if (totalDist != other.totalDist) return totalDist > other.totalDist;
        return maxEdge > other.maxEdge;
    }

    bool operator<(const Cost& other) const {
        if (totalDist != other.totalDist) return totalDist < other.totalDist;
        return maxEdge < other.maxEdge;
    }

    bool operator==(const Cost& other) const {
        return totalDist == other.totalDist && maxEdge == other.maxEdge;
    }
};


struct State {
    Cost cost;
    int u;

    bool operator>(const State& other) const {
        return cost > other.cost;
    }
};


struct Edge {
    int to;    
    int dist;   
    int id;     
};


struct EdgeInfo {
    int u;     
    int idx;   
};


int N, capital;
vector<Edge> adj[MAX_N];
unordered_map<int, EdgeInfo> edge_map;

Cost bestCost[MAX_N];
bool is_dirty;
priority_queue<State, vector<State>, greater<State>> pq;

void runDijkstra() {
    while (!pq.empty()) {
        State item = pq.top();
        pq.pop();

        int u = item.u;
        Cost currentCost = item.cost;

        if (currentCost > bestCost[u]) continue;

        for (const auto& edge : adj[u]) {
            if (edge.id == -1) continue;

            int v = edge.to;
            int w = edge.dist;

            Cost nextCost;
            nextCost.totalDist = currentCost.totalDist + w;
            nextCost.maxEdge = max(currentCost.maxEdge, w);

            if (nextCost < bestCost[v]) {
                bestCost[v] = nextCost;
                pq.push({ nextCost, v });
            }
        }
    }
}

void init(int n, int mCapital, int K, int mId[], int sCity[], int eCity[], int mDistance[]) {
    N = n;
    capital = mCapital;

    for (int i = 0; i < N; ++i) {
        adj[i].clear();
    }
    edge_map.clear();
    edge_map.reserve(24005);
    while (!pq.empty()) pq.pop();

    for (int i = 0; i < K; ++i) {
        adj[sCity[i]].push_back({ eCity[i], mDistance[i], mId[i] });
        edge_map[mId[i]] = { sCity[i], (int)adj[sCity[i]].size() - 1 };
    }

    is_dirty = true;
}

void add(int mId, int sCity, int eCity, int mDistance) {
    adj[sCity].push_back({ eCity, mDistance, mId });
    edge_map[mId] = { sCity, (int)adj[sCity].size() - 1 };

    if (is_dirty) return;

    if (bestCost[sCity].totalDist == INF) return;

    Cost nextCost;
    nextCost.totalDist = bestCost[sCity].totalDist + mDistance;
    nextCost.maxEdge = max(bestCost[sCity].maxEdge, mDistance);

    if (nextCost < bestCost[eCity]) {
        bestCost[eCity] = nextCost;
        while (!pq.empty()) pq.pop();
        pq.push({ nextCost, eCity });
        runDijkstra();
    }
}

void remove(int mId) {
    auto& info = edge_map[mId];

    int u = info.u;
    int idx = info.idx;

    Edge& targetEdge = adj[u][idx];

    int v = targetEdge.to;
    int w = targetEdge.dist;

    targetEdge.id = -1;
    //edge_map.erase(mId);

    if (is_dirty) return;
    if (bestCost[u].totalDist == INF) return;

    Cost cost_deleted;
    cost_deleted.totalDist = bestCost[u].totalDist + w;
    cost_deleted.maxEdge = max(bestCost[u].maxEdge, w);

    // 삭제된 간선이 최적 경로와 일치하는 비용을 가졌었다면 재계산 필요
    if (cost_deleted == bestCost[v]) {
        is_dirty = true;
    }
}

int calculate(int mCity) {
    if (mCity == capital) return 0;

    if (is_dirty) {
        for (int i = 0; i < N; ++i) bestCost[i] = { INF, INF };
        bestCost[capital] = { 0, 0 };

        while (!pq.empty()) pq.pop();
        pq.push({ {0, 0}, capital });

        runDijkstra();
        is_dirty = false;
    }

    if (bestCost[mCity].totalDist == INF) return -1;

    return bestCost[mCity].maxEdge;
}
