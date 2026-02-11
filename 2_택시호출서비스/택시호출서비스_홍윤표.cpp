#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <set>
#include <cstring>
using namespace std;

const int MAX_TAXI = 2000;
const int BUCKET_SIZE = 10;
const int INF = 2e9;

int N, M, L;

struct TAXI {
    int id = -1, x, y;
    int moveDistance;
    int rideDistance;

    bool operator<(const TAXI& other) const {
        if (rideDistance != other.rideDistance)
            return rideDistance > other.rideDistance; // 내림차순
        return id < other.id;
    }
};

struct Result {
    int mX, mY;
    int mMoveDistance;
    int mRideDistance;
};

TAXI taxies[MAX_TAXI];
set<TAXI> taxi_set;
unordered_set<int> bucket[BUCKET_SIZE][BUCKET_SIZE];

inline void convertToBucketCoord(int mx, int my, int* bx, int* by) {
    *bx = mx / L;
    *by = my / L;
}

inline int getDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

void init(int N, int M, int L, int mXs[], int mYs[])
{
    ::N = N; ::M = M; ::L = L;

    taxi_set.clear();

    for (int i = 0; i < BUCKET_SIZE; i++) {
        for (int j = 0; j < BUCKET_SIZE; j++) {
            bucket[i][j].clear();
            bucket[i][j].reserve(M / 100 + 5);
        }
    }

    memset(taxies, 0, sizeof(taxies));

    for (int i = 0; i < M; i++) {
        taxies[i] = { i, mXs[i], mYs[i], 0, 0 };

        int bx, by;
        convertToBucketCoord(mXs[i], mYs[i], &bx, &by);
        bucket[by][bx].insert(i);
        taxi_set.insert(taxies[i]);
    }
}

void findBestTaxi(int mSx, int mSy, int cbx, int cby, TAXI& best)
{
    int bestDist = INF;
    int bestId = -1;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int bx = cbx + dx;
            int by = cby + dy;

            if (bx < 0 || by < 0 || bx >= BUCKET_SIZE || by >= BUCKET_SIZE)
                continue;

            auto& taxi_ids = bucket[by][bx];

            for (int id : taxi_ids) {
                TAXI& cur = taxies[id];
                int dist = getDistance(mSx, mSy, cur.x, cur.y);

                if (dist <= L) {
                    if (dist < bestDist ||
                        (dist == bestDist && id < bestId)) {
                        bestDist = dist;
                        bestId = id;
                    }
                }
            }
        }
    }

    if (bestId == -1) {
        best.id = -1;
    }
    else {
        best = taxies[bestId];
    }
}

int pickup(int mSX, int mSY, int mEX, int mEY)
{
    int mbx, mby;
    convertToBucketCoord(mSX, mSY, &mbx, &mby);

    TAXI bestDriver;
    findBestTaxi(mSX, mSY, mbx, mby, bestDriver);

    if (bestDriver.id == -1)
        return -1;

    taxi_set.erase(bestDriver);

    int rideDist = getDistance(mSX, mSY, mEX, mEY);
    int moveDist = getDistance(bestDriver.x, bestDriver.y, mSX, mSY);

    int tbx, tby;
    convertToBucketCoord(bestDriver.x, bestDriver.y, &tbx, &tby);
    bucket[tby][tbx].erase(bestDriver.id);

    bestDriver.x = mEX;
    bestDriver.y = mEY;
    bestDriver.moveDistance += (rideDist + moveDist);
    bestDriver.rideDistance += rideDist;

    convertToBucketCoord(mEX, mEY, &tbx, &tby);
    bucket[tby][tbx].insert(bestDriver.id);

    taxies[bestDriver.id] = bestDriver;
    taxi_set.insert(bestDriver);

    return bestDriver.id + 1;
}

Result reset(int mNo)
{
    Result res;
    res.mX = res.mY = res.mMoveDistance = res.mRideDistance = -1;

    TAXI& target = taxies[mNo - 1];

    res.mX = target.x;
    res.mY = target.y;
    res.mMoveDistance = target.moveDistance;
    res.mRideDistance = target.rideDistance;

    if (target.moveDistance == 0 && target.rideDistance == 0)
        return res;

    taxi_set.erase(target);
    target.rideDistance = 0;
    target.moveDistance = 0;
    taxi_set.insert(target);

    return res;
}

void getBest(int mNos[])
{
    int cnt = 0;
    for (const TAXI& t : taxi_set) {
        mNos[cnt++] = t.id + 1;
        if (cnt == 5) break;
    }
}
