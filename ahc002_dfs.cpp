#include <bits/stdc++.h>
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define all(x) (x).begin(), (x).end()
typedef long long ll;
using namespace std;
int scores[50][50];
int tiles[50][50];
// 時間をDouble型で管理し、経過時間も取り出せるクラス
// https://atcoder.jp/contests/ahc002/submissions/47959533
class TimeKeeperDouble {
private:
  std::chrono::high_resolution_clock::time_point start_time_;
  double time_threshold_;

  double now_time_ = 0;

public:
  // 時間制限をミリ秒単位で指定してインスタンスをつくる。
  TimeKeeperDouble(const double time_threshold)
      : start_time_(std::chrono::high_resolution_clock::now()),
        time_threshold_(time_threshold) {}

  // 経過時間をnow_time_に格納する。
  void setNowTime() {
    auto diff = std::chrono::high_resolution_clock::now() - this->start_time_;
    this->now_time_ =
        std::chrono::duration_cast<std::chrono::microseconds>(diff).count() *
        1e-3; // ms
  }

  // 経過時間をnow_time_に取得する。
  double getNowTime() const { return this->now_time_; }

  // インスタンス生成した時から指定した時間制限を超過したか判定する。
  bool isTimeOver() const { return now_time_ >= time_threshold_; }
};
class State {
public:
  string path;
  set<int> visited_tiles;
  int score;
  int now_i, now_j;

  void print_state() {
    cout << "now_i: " << now_i << " now_j: " << now_j << endl
         << " score: " << score << " path: " << path << endl;
  }

  State(int si, int sj) {
    now_i = si;
    now_j = sj;
    score = scores[si][sj];
    path = "";
    visited_tiles.insert(tiles[si][sj]);
  }
};

int main() {
  auto time_keeper = TimeKeeperDouble(1900);
  int max_score = -1;
  string max_path = "U";

  int si, sj;
  cin >> si >> sj;
  rep(i, 50) rep(j, 50) cin >> tiles[i][j];
  rep(i, 50) rep(j, 50) cin >> scores[i][j];

  vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  vector<string> directions_char = {"U", "D", "L", "R"};

  stack<State> q;
  q.push(State(si, sj));

  while (!q.empty()) {
    time_keeper.setNowTime();
    if (time_keeper.isTimeOver()) {
      break;
    }
    State s = q.top();
    // s.print_state();
    q.pop();
    bool no_more_move = true;
    for (int i = 0; i < 4; i++) {
      int di = directions[i].first;
      int dj = directions[i].second;
      int ni = s.now_i + di;
      int nj = s.now_j + dj;
      if (ni < 0 || ni >= 50 || nj < 0 || nj >= 50)
        continue;
      if (s.visited_tiles.count(tiles[ni][nj]))
        continue;
      no_more_move = false;
      State next_s = s;
      next_s.now_i = ni;
      next_s.now_j = nj;
      next_s.score += scores[ni][nj];
      next_s.visited_tiles.insert(tiles[ni][nj]);
      next_s.path.append(directions_char[i]);
      q.push(next_s);
    }

    if (s.score > max_score && s.path != "") {
      max_score = s.score;
      max_path = s.path;
    }
  }

  cout << max_path << endl;

  return 0;
}