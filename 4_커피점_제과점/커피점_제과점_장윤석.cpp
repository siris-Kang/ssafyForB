#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cstring> 

#define MAX_NODE 10005

using namespace std;

struct Info {
    int now_node;
    int cost;
    int type;

    bool operator < (const Info& next) const {
        return cost > next.cost;
    }
};

vector<Info> graph[MAX_NODE];

int dist[2][MAX_NODE];
int station_type[MAX_NODE]; 

int NN;
int min_result;

void init(int N, int K, int sBuilding[], int eBuilding[], int mDistance[]) {
    NN = N;
    for (int i = 0; i < NN; i++) {
        graph[i].clear();
    }

    for (int i = 0; i < K; i++) {
        graph[sBuilding[i]].push_back({ eBuilding[i], mDistance[i], 0 }); 
        graph[eBuilding[i]].push_back({ sBuilding[i], mDistance[i], 0 });
    }
}

void add(int sBuilding, int eBuilding, int mDistance) {
    graph[sBuilding].push_back({ eBuilding, mDistance, 0 });
    graph[eBuilding].push_back({ sBuilding, mDistance, 0 });
}

int calculate(int M, int mCoffee[], int P, int mBakery[], int R) {

    for (int i = 0; i < NN; i++) {
        dist[0][i] = INT_MAX;
        dist[1][i] = INT_MAX;
        station_type[i] = 0;
    }

    priority_queue<Info> pq;
    min_result = INT_MAX;

  
    for (int i = 0; i < M; i++) {
        int node = mCoffee[i];
        station_type[node] = 1;
        dist[0][node] = 0;
        pq.push({ node, 0, 0 }); 
    }

    for (int i = 0; i < P; i++) {
        int node = mBakery[i];
        station_type[node] = 2; 
        dist[1][node] = 0;
        pq.push({ node, 0, 1 }); 
    }

    while (!pq.empty()) {
        Info now = pq.top();
        pq.pop();

        if (now.cost >= min_result) continue;
        if (dist[now.type][now.now_node] < now.cost) continue;

        int other_type = 1 - now.type;

        if (station_type[now.now_node] == 0 && dist[other_type][now.now_node] != INT_MAX) {
            int current_total = now.cost + dist[other_type][now.now_node];
            if (current_total < min_result) {
                min_result = current_total;
            }
        }

        for (int i = 0; i < graph[now.now_node].size(); i++) {
            Info next = graph[now.now_node][i];

            if (station_type[next.now_node] == now.type + 1) continue;

            int nextcost = now.cost + next.cost;

            if (nextcost > R) continue;

            if (nextcost >= min_result) continue;

            if (dist[now.type][next.now_node] > nextcost) {
                dist[now.type][next.now_node] = nextcost;
                pq.push({ next.now_node, nextcost, now.type });
            }
        }
    }

    if (min_result == INT_MAX) return -1;
    return min_result;
}