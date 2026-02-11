#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
using namespace std;

const int MAX_WORD = 50005;
const int MAX_N = 20000;
const int MAX_M = 1000;
const int PAGE_SIZE = 500;
int N, M, page_num;

struct Word {
    int s, e, len, line_num;
    bool valid = false;
};

struct space {
    int s, len, e;
    space(int _s, int _len) {
        s = _s; len = _len; e = _s + _len - 1;
    }
};

Word words[MAX_WORD];
int lines_max[MAX_N]; // 라인당 최댓값
int pages_max[MAX_N / PAGE_SIZE];
vector<space> spaces[MAX_N]; // 각 라인당 공간들

void init(int N, int M)
{
    ::N = N; ::M = M;
    memset(words, 0, sizeof(words));
    page_num = (N+PAGE_SIZE - 1) / PAGE_SIZE;
    fill(lines_max, lines_max + N, M);
    fill(pages_max, pages_max + page_num, M);
    
    for (int i = 0; i < N; i++) {
        spaces[i].clear();
        spaces[i].push_back({ 0,M });
    }

}
void print() {
    for (int i = 0; i < N; i++) {
        cout << "=========line " << i  << "=========" << '\n';
        for (auto it = spaces[i].begin(); it != spaces[i].end(); it++) {
            cout << it->s << " to " << it->e << '\n';
        }
        cout << "line max is " << lines_max[i];
        cout << '\n';
    }
}
int findAvailablePageNum(int mLen) {
    for (int i = 0; i < page_num; i++) {
        if (pages_max[i] >= mLen) return i;
    }
    return -1;
}
int findBestLine(int mLen, int t_page) {
    int start = PAGE_SIZE * t_page;

    for (int i = 0; i < PAGE_SIZE; i++) {
        int idx = i + start;
        if (lines_max[idx] >= mLen) return idx;
    }
    return -1;
}
void insertWord(vector<space> &line,int mLen,int mId) {
    for (auto it = line.begin(); it != line.end(); it++) {
        // 앞에서 부터 탐색
        int len = it->len; // 공간 길이
        if (len >= mLen) {
            words[mId].s = it->s;
            words[mId].len = mLen;
            words[mId].e = words[mId].s + mLen - 1;
            words[mId].valid = true;

            it->s = words[mId].e + 1;
            it->len = len - mLen;
            it->e = it->s + it->len - 1;
            if (it->len == 0) line.erase(it);
            break;
        }
    }
    // word 등록
}
void eraseWord(vector<space>& line, Word& word) {
    int w_s = word.s;
    int w_e = word.e;


    space new_space(w_s, word.len);
    line.push_back(new_space);
    sort(line.begin(), line.end(), [&](const space& a, const space& b) {
        return a.s < b.s;
        });
    vector<space> merged;
    space last(line[0]);
    
    for (int i = 1; i < line.size(); i++) {
        auto cur = line[i];
        if (last.e + 1 >= cur.s) {
            last.e = cur.e;
            last.len = last.e - last.s + 1;
        }
        else {
            merged.push_back(last);
            last = cur;
        }
    }
    merged.push_back(last);
    if (merged.empty()) merged.push_back(new_space);
    line = merged;


}
void updatePageMax(int target_page) {
    int start = PAGE_SIZE * target_page;
    pages_max[target_page] = 0;
    for (int i = 0; i < PAGE_SIZE; i++) {
        int idx = start + i;
        pages_max[target_page] = max(lines_max[idx], pages_max[target_page]);
    }
}

void updateLineMax(vector<space>& line, int target_line) {
    lines_max[target_line] = 0;
    for (auto it = line.begin(); it != line.end(); it++) {
        lines_max[target_line] = max(it->len, lines_max[target_line]);
    }
}

int writeWord(int mId, int mLen)
{
    //cout << "writeWord(" << mId << ", " << mLen << ")\n";
    int target_page = findAvailablePageNum(mLen);
    if(target_page == -1) return -1;
    int target_line = findBestLine(mLen, target_page);
    if (target_line == -1) return -1;

    // 이제 타겟 라인 찾았으니 영역전개
    vector<space>& line = spaces[target_line];
    insertWord(line,mLen,mId);
    // word 등록
    words[mId].line_num = target_line;
    
    // 최댓값 갱신(line)
    updateLineMax(line, target_line);

    // 최댓값 갱신(page)
    updatePageMax(target_page);
    //print();

    //cout <<"answer -> " <<  target_line << '\n';

    return target_line;
}

int eraseWord(int mId)
{
    //cout << "지우기(" << mId << ")\n";

    if (mId > 50000) return -1;
    if (!words[mId].valid) return -1;

    Word& word = words[mId];
    word.valid = false;
    int target_line = word.line_num;
    int target_page = target_line / PAGE_SIZE;
    vector<space>& line = spaces[target_line];
    eraseWord(line, word);

    // 최댓값 갱신(line)
    updateLineMax(line, target_line);

    // 최댓값 갱신(page)
    updatePageMax(target_page);
    //cout << "answer -> " << target_line << '\n';
    //print();

    return target_line;
}
	
