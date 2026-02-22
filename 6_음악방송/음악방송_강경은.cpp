#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

struct SegTree {
    int n;
    int len;
    int SIZE;

    vector<int> tree;

    SegTree() : n(0), len(1), SIZE(2) {}

    SegTree(int N) {
        n = N;
        len = 1;
        while (len < n)
            len *= 2;
        SIZE = len * 2;

        tree.assign(SIZE, 0);
    }

    void update(int idx, int list_s, int list_e, int pos, int val) {
        if (pos < list_s || pos > list_e)
            return;
        if (list_s == list_e) {
            tree[idx] += val;
            return;
        }

        int mid = (list_s + list_e) / 2;
        update(idx * 2, list_s, mid, pos, val);
        update(idx * 2 + 1, mid + 1, list_e, pos, val);

        tree[idx] = tree[idx * 2] + tree[idx * 2 + 1];
    }
    void update(int pos, int val) {
        update(1, 0, len - 1, pos, val);
    }

    int get(int idx, int list_s, int list_e, int ts, int te) {
        if (te < list_s || ts > list_e)
            return 0;
        if (ts <= list_s && list_e <= te)
            return tree[idx];

        int mid = (list_s + list_e) / 2;
        int L = get(idx * 2, list_s, mid, ts, te);
        int R = get(idx * 2 + 1, mid + 1, list_e, ts, te);

        return L + R;
    }
    int get(int ts, int te) {
        return get(1, 0, len - 1, ts, te);
    }
};

struct Partner {
    int start;
    int end;
};

static const int MAXT = 2000000;
static const int N = MAXT + 3;

int musicTime;
SegTree stStart;
SegTree stEnd;
unordered_map<int, Partner> partners;

void init(int inputMusicTime)
{
    partners.clear();

    musicTime = inputMusicTime;
    stStart = SegTree(N);
    stEnd = SegTree(N);
}

void add(int mID, int mStart, int mEnd)
{
    auto it = partners.find(mID);
    if (it != partners.end()) {
        int os = it->second.start;
        int oe = it->second.end;

        stStart.update(os, -1);
        stEnd.update(oe + 1, -1);

        it->second = { mStart, mEnd };
    }
    else {
        partners[mID] = { mStart, mEnd };
    }

    stStart.update(mStart, 1);
    stEnd.update(mEnd + 1, 1);
}

void remove(int mID)
{
    auto it = partners.find(mID);
    if (it == partners.end())
        return;

    int s = it->second.start;
    int e = it->second.end;

    stStart.update(s, -1);
    stEnd.update(e + 1, -1);

    partners.erase(it);
}

int getCnt(int mBSTime)
{
    int start_emp = stStart.get(0, mBSTime);
    int end_emp = stEnd.get(0, mBSTime + musicTime);

    return start_emp - end_emp;
}