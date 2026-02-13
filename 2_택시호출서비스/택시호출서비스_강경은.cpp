#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <cmath>

using namespace std;

struct Result
{
	int mX, mY;
	int mMoveDistance;
	int mRideDistance;
};

struct Taxi {
	int num; // Taxi 번호
	int x, y; // Taxi 위치
	int move; // 운행거리
	int cus_ride; // 손님을 태우고 이동한 거리
};

struct Cmp {
	bool operator()(const Taxi* a, const Taxi* b) const {
		if ((*a).cus_ride == b->cus_ride) {
			return a->num < b->num;
		}
		return a->cus_ride > b->cus_ride;
	}
};

int N, M, L;
vector<Taxi> taxis; // Taxi 번호는 1부터
set<Taxi*, Cmp> taxi_set;

vector<vector<unordered_set<int>>> buckmap;


void init(int inputN, int inputM, int inputL, int mXs[], int mYs[])
{
	N = inputN;
	M = inputM;
	L = inputL;

	taxis.clear();
	taxi_set.clear();
	buckmap.clear();

	taxis.resize(M + 1);
	buckmap.resize(10, vector<unordered_set<int>>(10));

	for (int i = 1; i <= M; i++) {
		taxis[i].num = i;
		taxis[i].x = mXs[i - 1];
		taxis[i].y = mYs[i - 1];
		taxis[i].move = 0;
		taxis[i].cus_ride = 0;
		taxi_set.insert(&taxis[i]);

		int nx = taxis[i].x / L;
		int ny = taxis[i].y / L;
		buckmap[nx][ny].insert(i);
	}
	return;
}

int pickup(int mSX, int mSY, int mEX, int mEY)
{
	// start 주변 9개 구역 보면서 L보다 작으면서
	// 가장 번호가 작은 택시 구하기
	int min_dist = L + 1;
	int picked_taxi = 2002;
	int bef_taxi_x = -1;
	int bef_taxi_y = -1;

	int nowx = mSX / L;
	int nowy = mSY / L;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) { // 주변 9개 구역 확인
			int nx = nowx + i;
			int ny = nowy + j;
			if (nx < 0 || nx >= 10 || ny < 0 || ny >= 10)
				continue;

			// buckmap
			for (auto iter = buckmap[nx][ny].begin(); iter != buckmap[nx][ny].end(); iter++) {
				int taxi_num = *iter;
				Taxi now = taxis[taxi_num];
				int dist = abs(now.x - mSX) + abs(now.y - mSY);
				if (dist < min_dist || (dist == min_dist && taxi_num < picked_taxi)) {
					min_dist = dist;
					picked_taxi = taxi_num;
					bef_taxi_x = nx;
					bef_taxi_y = ny;
				}
			}
		}
	}
	if (min_dist > L) { // 호출 가능한 taxi가 없는 경우
		return -1;
	}

	// set과 buckmap에서 삭제
	taxi_set.erase(&taxis[picked_taxi]); // 값 변경할거니까 set에서 지우기
	buckmap[bef_taxi_x][bef_taxi_y].erase(picked_taxi); // 택시 이동할거니까 buckmap에서 지우기

	// 이동하는 taxi 정보 업데이트
	taxis[picked_taxi].x = mEX;
	taxis[picked_taxi].y = mEY;
	int cus_ride_dist = abs(mSX - mEX) + abs(mSY - mEY);
	taxis[picked_taxi].move += min_dist + cus_ride_dist;
	taxis[picked_taxi].cus_ride += cus_ride_dist;

	// set과 buckmap에 다시 추가
	int nx = mEX / L;
	int ny = mEY / L;
	taxi_set.insert(&taxis[picked_taxi]);
	buckmap[nx][ny].insert(picked_taxi);

	return picked_taxi;
}

Result reset(int mNo)
{
	Result res;
	res.mX = taxis[mNo].x;
	res.mY = taxis[mNo].y;
		
	res.mMoveDistance = taxis[mNo].move;
	res.mRideDistance = taxis[mNo].cus_ride;

	taxi_set.erase(&taxis[mNo]);

	taxis[mNo].move = 0;
	taxis[mNo].cus_ride = 0;

	taxi_set.insert(&taxis[mNo]);

	return res;
}

void getBest(int mNos[])
{
	int cnt = 0;
	for (auto iter = taxi_set.begin(); iter != taxi_set.end(); iter++) {
		mNos[cnt] = (*iter)->num;
		cnt++;
		if (cnt == 5)
			break;
	}
	return;
}