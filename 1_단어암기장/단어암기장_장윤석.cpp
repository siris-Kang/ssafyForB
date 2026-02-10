#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

struct Word {

    int id;
    int bucket_num;
    int sr;
    int sc;
    int len;
};

unordered_map<int, Word> mId_to_word;  // 실제 id, index

const int MAX_N = 20'005;
const int MAX_M = 1'005;
const int BUCKET_SIZE = 200;

bool notebook[MAX_N][MAX_M];
int bucket_max[MAX_N / BUCKET_SIZE + 5];
int row_max[MAX_N];

int NN = 0;
int MM = 0;
int bucket_cnt = 0;

void init(int N, int M)
{
    mId_to_word.clear();
    NN = N; MM = M;
    bucket_cnt = NN / 200 + 1;

    for (int r = 0; r < NN; r++) {
        for (int c = 0; c < MM; c++)
            notebook[r][c] = false;
    }
    for (int b = 0; b < bucket_cnt; b++)
        bucket_max[b] = MM;
    
    for (int r = 0; r < NN; r++)
        row_max[r] = MM;
}

void update_max(int near_row_num, int start_row, int row_len, int now_bucket_num) {

    // 한행에서 지금 연달아 있는 최대 개수를 확인 후 갱신해준다.
    int tmp_len = 0;
    int max_tmp_len = 0;
    for (int c = 0; c < MM; c++) {
        if (notebook[near_row_num][c] == 0) {
            tmp_len++;
            max_tmp_len = max(max_tmp_len, tmp_len);
        }
        else tmp_len = 0;
    }
    row_max[near_row_num] = max_tmp_len;

    // 현재 버킷에서 지금 연달아 있는 최대 개수를 확인 후 갱신한다.
    int bucket_max_len = 0;
    for (int sr = start_row; sr < start_row + row_len; sr++) {
        if (row_max[sr] == MM) {
            bucket_max[now_bucket_num] = MM;
            break;
        }
        else {
            bucket_max_len = max(bucket_max_len, row_max[sr]);
            bucket_max[now_bucket_num] = bucket_max_len;
        }
    }
}
int writeWord(int mId, int mLen)
{
    int now_bucket_num = 0;
    Word w;
    for (int b = 0; b < bucket_cnt; b++) {

        now_bucket_num = b;
        
        // bucket_max 배열에서 mLen보다 같거나 큰 값이 있다면
        if (mLen <= bucket_max[now_bucket_num]) {
            // row_max 배열에서 bucket에 해당되는 행 범위를 찾는다.
            int start_row = 200 * now_bucket_num;
            int row_len = 200;
            if (now_bucket_num == bucket_cnt - 1) row_len = NN % 200;

            // 제일 행번호가 가까운 것을 찾는다.
            int near_row_num = 0;
            for (int sr = start_row; sr < start_row + row_len; sr++) {
                if (mLen <= row_max[sr]) {
                    near_row_num = sr;
                    break;
                }
            }
            // 단어를 넣을 수 있는 곳이 여러개 있을 수 있다.
            int start_sc = -1;
            int tmp_len = 0;
            
            for (int c = 0; c < MM; c++) {
                if (notebook[near_row_num][c] == false) {
                    tmp_len++;
                    if (tmp_len == mLen) {
                        start_sc = c - mLen + 1;
                        break;
                    }
                }
                else {
                    tmp_len = 0;
                }
            }
            // 단어를 넣고 갱신해준다. 
            if (start_sc != -1) {
                for (int cc = start_sc; cc < start_sc + mLen; cc++) {
                    notebook[near_row_num][cc] = true;
                }
                w.id = mId;
                w.bucket_num = now_bucket_num;
                w.sr = near_row_num;
                w.sc = start_sc;
                w.len = mLen;
                mId_to_word[mId] = w;

                update_max(near_row_num, start_row, row_len, now_bucket_num);
                return near_row_num;
            }
        }
    }
    return -1;
}

int eraseWord(int mId)
{
    int ans = 0;
    // 만약에 mId가 잇다면?
    if (mId_to_word.find(mId) == mId_to_word.end()) {
        return -1;
    }
    Word w = mId_to_word[mId];
    for (int sc = w.sc; sc < w.sc + w.len; sc++) {
        notebook[w.sr][sc] = 0;
    }

    int start_row = 200 * w.bucket_num;
    int row_len = 200;
    if (w.bucket_num == bucket_cnt - 1) row_len = NN % 200;

    update_max(w.sr, start_row, row_len, w.bucket_num);
    mId_to_word.erase(mId);
    ans = w.sr;

    return ans;
}