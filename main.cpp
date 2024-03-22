#include <bits/stdc++.h>
using namespace std;

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
        1e-3;  // ms
  }

  // 経過時間をnow_time_に取得する。
  double getNowTime() const { return this->now_time_; }

  // インスタンス生成した時から指定した時間制限を超過したか判定する。
  bool isTimeOver() const { return now_time_ >= time_threshold_; }
};


// 乱数を生成するクラス
class Random {
public:
    std::mt19937 mt_;  // シード0でメルセンヌツイスターの乱数生成器を初期化
    // 0以上1.0未満の実数の範囲の乱数生成
    uniform_real_distribution<double> dd_{0, 1.0};

    // seedを指定して初期化
    Random(const int seed = 0) : mt_(std::mt19937(seed)) {}

    // 0以上m未満の整数の範囲の乱数
    inline int nextInt(const int m) {
        uniform_int_distribution<int> di(0, m - 1);
        return di(mt_);
    }

    // 0以上1.0未満の実数の範囲の乱数
    inline double nextDouble() { return dd_(mt_); }

    // 0以上1.0未満の実数の範囲の乱数のlog。焼きなまし法で使いやすい。
    inline double nextLog() { return log(dd_(mt_)); }
};

Random rnd{};

const int N = 50;
const int M = 2500;
int start_i, start_j;
int tiles[51][51], points[51][51];

int dx[4] = {-1, 1, 0, 0}, dy[4] = {0, 0, -1, 1};

class dfsSolver {
public:
    array<bool, M> visited;
    vector<pair<int, int>> path, best_path;
    int score, best_score;
    int remaining_search_cnt;

    void start() {
        best_path.clear();
        score = 0;
        visited.fill(0);
        path.clear();
        remaining_search_cnt = 4000;
        dfs(start_i, start_j);
    }

    void dfs(int crd_i, int crd_j) {
        path.push_back({crd_i, crd_j});
        score += points[crd_i][crd_j];
        visited[tiles[crd_i][crd_j]] = 1;
        
        if (best_score < score) {
            best_score = score;
            best_path = path;
        }

        remaining_search_cnt--;
        if (remaining_search_cnt == 0) return;

        for (int d = 0; d < 4; d++) {
            int ni = crd_i + dx[d], nj = crd_j + dy[d];
            if (ni < 0 || N <= ni || nj < 0 || N <= nj) continue;
            if (visited[tiles[ni][nj]]) continue;

            dfs(ni, nj);
            if (remaining_search_cnt == 0) return;
        }
        path.pop_back();
        score -= points[crd_i][crd_j];
        visited[tiles[crd_i][crd_j]] = 0;
    }
};

class dfsPartSolver {
public:
    array<bool, M> visited_;
    vector<pair<int, int>> path_, best_path_;
    int score_ = 0, best_score_ = 0;
    int remaining_search_cnt_ = 0;
    pair<int, int> goal_;

    void start(pair<int, int> start, pair<int, int> goal, array<bool, M> visited, int remaining_search_cnt) {
        goal_ = goal;
        best_path_.clear();
        best_score_ = 0;
        score_ = 0;
        remaining_search_cnt_ = remaining_search_cnt;
        visited_ = visited;
        path_.clear();
        dfs(start);
    }

    void dfs(pair<int, int> crd) {
        auto [i, j] = crd;
        if (!visited_[tiles[i][j]]) {
            path_.push_back(crd);
            score_ += points[i][j];
            visited_[tiles[i][j]] = 1;
        }
        remaining_search_cnt_--;
        if (remaining_search_cnt_ == 0) return;

        if (crd != goal_) {
            vector<pair<int, int>> legal_next_crds;
            for (int d = 0; d < 4; d++) {
                int ni = i + dx[d], nj = j + dy[d];
                if (ni < 0 || N <= ni || nj < 0 || N <= nj) continue;
                if (visited_[tiles[ni][nj]]) continue;
                
                if (make_pair(ni, nj) == goal_) {
                    best_score_ = score_;
                    best_path_ = path_;
                    remaining_search_cnt_ = 0;
                    return;
                } else {
                    legal_next_crds.push_back({ni, nj});
                }
            }
            shuffle(legal_next_crds.begin(), legal_next_crds.end(), rnd.mt_);
            for (auto [ni, nj]: legal_next_crds) {
                if (visited_[tiles[ni][nj]]) continue;
                dfs({ni, nj});
                if (remaining_search_cnt_ == 0) return;
            }
        }
        path_.pop_back();
        score_ -= points[i][j];
        visited_[tiles[i][j]] = 0;
    }
};

class State {
    vector<pair<int, int>> path;

    void solve_first_() {
        dfsSolver dfssolver;
        dfssolver.start();
        path = dfssolver.path;
    }

public:
    void hillClimbing(int64_t time_threshold) {
        auto timekeeper = TimeKeeperDouble(time_threshold);

        solve_first_();

        array<bool, M> visited;
        dfsPartSolver dfs_part_solver;
        for (int i = 0; i < path.size(); i++) {
            visited[tiles[path[i].first][path[i].second]] = 1;
        }

        while (1) {
            timekeeper.setNowTime();
            if (timekeeper.isTimeOver()) break;

            int delete_path_len = rnd.nextInt((int)(1 + path.size() * 0.05));
            int start_path_id = rnd.nextInt(path.size() - delete_path_len);
            int end_path_id = start_path_id + delete_path_len;

            auto next_visiteds = visited;

            int remaining_search_cnt = 4 * delete_path_len;

            int now_score = 0;
            for (int i = start_path_id + 1; i <= end_path_id - 1; i++) {
                now_score += points[path[i].first][path[i].second];
                next_visiteds[tiles[path[i].first][path[i].second]] = 0;
            }
            dfs_part_solver.start(path[start_path_id], path[end_path_id],
                                    next_visiteds, remaining_search_cnt);

            int next_score = dfs_part_solver.best_score_;
            int diff = next_score - now_score;
            if (dfs_part_solver.best_path_.size() > 0 && diff > 0) {
                // visitedsの更新
                visited = dfs_part_solver.visited_;

                // 今回生成した部分パスを挿入した全体パスを生成
                vector<pair<int, int>> transitioned_path;
                for (int i = 0; i <= start_path_id; i++) {
                    transitioned_path.emplace_back(path[i]);
                }
                for (int i = 0; i < dfs_part_solver.best_path_.size(); i++) {
                    transitioned_path.emplace_back(dfs_part_solver.best_path_[i]);
                }
                for (int i = end_path_id; i < path.size(); i++) {
                    transitioned_path.emplace_back(path[i]);
                }
                path = transitioned_path;
            }
            
        }
    }
public:
    void input() {
        cin >> start_i >> start_j;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) cin >> tiles[i][j];
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) cin >> points[i][j];
        }
    }
    void solve() {
        dfsSolver solver;
        solver.start();
        path = solver.path;
    }
    void output() {
        string ans;
        for (int i = 1; i < path.size(); i++) {
            int diff_i = path[i].first - path[i - 1].first, diff_j = path[i].second - path[i - 1].second;
            if (diff_i == 1) ans.push_back('D');
            else if (diff_i == -1) ans.push_back('U');
            else if (diff_j == 1) ans.push_back('R');
            else ans.push_back('L');
        }
        cout << ans << endl;
    }
};

int main() {
    State state;
    state.input();
    state.hillClimbing(1950);
    state.output();
}