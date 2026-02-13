#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <set>
#include <vector>

#define MAX_GENRE 5

using namespace std;

struct Movie {

    int mID;
    int mGenre;
    int mTotal; // 전역 총점 
    int initial_t;  // 등록 시간
    bool isRemoved; // 삭제 여부
};


// 정렬을 위한 구조체
struct MovieRank {

    int mID;
    int mTotal;
    int initial_t;

    // 내림차순 정렬 (총점 > 최신순 > ID)
    bool operator < (const MovieRank& next) const {
        if (mTotal == next.mTotal) {
            if (initial_t == next.initial_t) {
                return mID < next.mID; // ID는 구분을 위한 key
            }
            return initial_t > next.initial_t; // 최신순
        }
        return mTotal > next.mTotal; // 총점 높은순
    }
};

struct UserHistory {

    int mID;
    int mRating;
    int watchtime;
};


struct RESULT
{
    int cnt;
    int IDs[5];
};

unordered_map<int, Movie> movie_map;
unordered_map<int, vector<UserHistory>> user_history;
set<MovieRank> genre_rank[MAX_GENRE + 1];

// user_watched[uID] -> {mID1, mID2 ,,,}
unordered_map<int, unordered_set<int>> user_watched;

int global_time = 0;

void init(int N)
{
    movie_map.clear();
    user_history.clear();
    user_watched.clear();
    for (int i = 0; i <= MAX_GENRE; i++) 
        genre_rank[i].clear();
    global_time = 0;
}

int add(int mID, int mGenre, int mTotal)
{
    // 영화 등록에 성공하는 경우
    if (movie_map.find(mID) == movie_map.end()) {

        Movie now_movie = { mID, mGenre, mTotal, global_time++, false};
        movie_map[mID] = now_movie;

        // 장르별 mID 관리하는 SET에서도 업데이트 한다.
        genre_rank[mGenre].insert({mID, mTotal, now_movie.initial_t });

        // 전체 랭킹 set(0번)에도 추가
        genre_rank[0].insert({mID, mTotal, now_movie.initial_t});
        return 1;
    }
    // 영화 등록에 실패하는 경우
    else {
        return 0;
    }
}

int erase(int mID)
{
    auto it = movie_map.find(mID);

    if (it == movie_map.end() || it->second.isRemoved == true) {
        return 0;
    }

    Movie& now_movie = it->second;
    MovieRank target = { mID, now_movie.mTotal, now_movie.initial_t };
    genre_rank[now_movie.mGenre].erase(target);
    genre_rank[0].erase(target);

    now_movie.isRemoved = true;

    return 1;
    
}

int watch(int uID, int mID, int mRating)
{

    auto it = movie_map.find(mID);

    if ((it == movie_map.end() || it->second.isRemoved == true) || (user_watched[uID].count(mID))) {
        return 0;
    }

    user_history[uID].push_back({ mID, mRating, global_time++ });
    user_watched[uID].insert(mID);
    

    Movie& now_movie = it->second;
    MovieRank old_rank = { mID, now_movie.mTotal, now_movie.initial_t };

    genre_rank[now_movie.mGenre].erase(old_rank);
    genre_rank[0].erase(old_rank);


    now_movie.mTotal += mRating;

    genre_rank[now_movie.mGenre].insert({ mID, now_movie.mTotal, now_movie.initial_t });
    genre_rank[0].insert({ mID, now_movie.mTotal, now_movie.initial_t });


    return 1;
}

RESULT suggest(int uID) {

    RESULT res;
    res.cnt = 0;

    int target_genre = 0;
    vector<UserHistory>& history = user_history[uID];

    if (!history.empty()) {
    
        int check_cnt = 0;
        int best_rating = -1;
        int best_idx = -1; 


        for (int i = history.size() - 1; i >= 0; i--) {

            if (check_cnt == 5) break;
            if (movie_map[history[i].mID].isRemoved) continue;

            check_cnt++;

            // 평점이 더 높거나, (평점이 같으면 더 최근에 본 것이 우선 -> 뒤에서부터 도니까 자연스럽게 해결)
            if (history[i].mRating > best_rating) {
                best_rating = history[i].mRating;
                best_idx = i;
            }
        }

        if (best_idx != -1) {
            target_genre = movie_map[history[best_idx].mID].mGenre;
        }
    }

    vector<int> candidates;

    for (auto it = genre_rank[target_genre].begin(); it != genre_rank[target_genre].end(); ++it) {
        if (candidates.size() >= 5) break;

        int mID = it->mID;

        if (user_watched[uID].count(mID)) continue;

        candidates.push_back(mID);
    }


    res.cnt = candidates.size();
    for (int i = 0; i < res.cnt; i++) res.IDs[i] = candidates[i];
    return res;
}

