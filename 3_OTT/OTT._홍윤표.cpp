#include <vector>
#include <unordered_map>
#include <queue>
#include <deque>
#include <algorithm>
#include <set>
//#include 
#define MAX_MOVIE 10000
#define MAX_USER 1000
#define MAX_GENRE 5


using namespace std;
typedef long long ll;
typedef pair<int, int> pii;
struct RESULT {
    int cnt;
    int IDs[5];
};


struct movie {
    int mID;
    ll total, made;
    int genre;
    vector<int> userRate; // 각 유저가 준 평점
}; 


struct user {
    // mID, rating
    deque<pii> latest;
};

struct cmpRecommend {
    bool operator()(const movie& a, const movie& b) const {
        if (a.total != b.total) return a.total > b.total;
        if (a.made != b.made)  return a.made > b.made;
        return a.mID < b.mID; // ID 구별
    }
};


int date;
unordered_map<int, movie> movieTable;
unordered_map<int, user> userTable;

// 전체 영화 순서
set<movie, cmpRecommend> movies_set;
// 장르에 따른 순서.
vector<set<movie, cmpRecommend>> genres_movies_set;
int user_N;


// 초기화
void init(int N) {
    date = 0;
    user_N = N;
    movieTable.clear();
    userTable.clear();
    userTable.reserve(MAX_USER);
    for (int i = 1; i <= N; i++)
        userTable[i] = user();
    movieTable.reserve(MAX_MOVIE);
    movies_set.clear();
    genres_movies_set.assign(MAX_GENRE + 1, {});
}
bool findMovie(const int& mID) {
    return movieTable.find(mID) != movieTable.end();
}
// 영화 추가
int add(int mID, int mGenre, int mTotal) {
    int result;
    if (findMovie(mID)) result = 0;
    else {
        result = 1;
        movie m;
        m.mID = mID;
        m.made = date++;
        m.total = mTotal;
        m.genre = mGenre;
        m.userRate.assign(user_N + 1, 0);

        movieTable[mID] = m;
        movies_set.insert(m);
        genres_movies_set[mGenre].insert(m);
    }
    //cout << result << '\n';

    return result;
}


int erase(int mID) {
    int result;
    if (findMovie(mID)) {
        movie m = movieTable[mID];
        movieTable.erase(m.mID);
        movies_set.erase(m);
        genres_movies_set[m.genre].erase(m);
        result = 1;
    }
    else result = 0;
    //cout << result << '\n';

    return result;
}


int watch(int uID, int mID, int mRating) {
    int result;
    user& u = userTable[uID];
    // 영화가 존재하지 않음
    if (!findMovie(mID)) result = 0;
    else {
        // movieTable에서 기존 영화 가져오기
        movie m = movieTable[mID];

        // 이미 시청했는지 확인
        if (m.userRate[uID] != 0) {
            result = 0;
        }
        else {
            result = 1;

            // 1. set에서 기존 객체 제거 (정렬 유지)
            movies_set.erase(m);
            genres_movies_set[m.genre].erase(m);
            // 2. movie 정보 업데이트
            m.total += mRating;
            m.userRate[uID] = mRating;

            u.latest.push_front({ mID, mRating });

            movieTable[mID] = m;
            //movie_genre_Table[m.genre][mID] = m;

            // 4. set에 다시 삽입 -> 정렬 유지
            movies_set.insert(m);
            genres_movies_set[m.genre].insert(m);
        }

    }
    //cout << result << '\n';
    return result;
}


RESULT suggest(int uID) {
    RESULT res;
    res.cnt = 0;
    int cnt = 0;

    user& usr = userTable[uID];
    // 0이라면 본 영화 없는 것
    int max_rating = 0;
    int max_genre = 0;
    int scanned = 0;

    auto it = usr.latest.begin();

    while (it != usr.latest.end() && scanned < 5) {
        int mID = it->first;
        int rating = it->second;

        if (!findMovie(mID)) { //영화가 사라졌다면
            it = usr.latest.erase(it); // 본 거에서 제거
            continue;
        }
        else {
            if (rating > max_rating) {
                max_rating = rating;
                max_genre = movieTable[mID].genre;
            }
            scanned++;
            it++;
        }
    }
    // 전체 영화 추천 경우

    // 이미 평점과 시간순으로 정렬된 상태
    if (max_rating == 0) {
        for (auto it = movies_set.begin(); it != movies_set.end(); it++) {
            if (cnt == 5) break;
            // 이미 삭제된 영화
            if (!findMovie(it->mID)) continue;
            // 이미 본 영화
            if (it->userRate[uID] != 0) continue;
            cnt++;
            res.IDs[res.cnt++] = it->mID;
        }
    }
    else {
        for (auto it = genres_movies_set[max_genre].begin();
            it != genres_movies_set[max_genre].end(); it++) {
            if (cnt == 5) break;
            // 이미 삭제된 영화
            if (!findMovie(it->mID)) continue;
            // 이미 본 영화
            if (it->userRate[uID] != 0) continue;
            cnt++;
            res.IDs[res.cnt++] = it->mID;
        }
    }
    return res;
}
