// 焼きなまし法のサンプルコード

// VSCODEの拡張機能C/C++の設定
// "C_Cpp.clang_format_sortIncludes": true,
// "C_Cpp.clang_format_style": "Google" でフォーマット済み

#include <algorithm>  //shuffleに必要
#include <array>
#include <chrono>    // 時間計測
#include <iostream>  //入出力
#include <random>
#include <vector>

using namespace std;  // 自己責任で使おう

// 時間をDouble型で管理し、経過時間も取り出せるクラス
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

constexpr int H = 50;         // 盤面の縦方向の大きさ
constexpr int W = 50;         // 盤面の横方向の大きさ
constexpr int TILE_N = 2500;  // タイルの総数。2500枚よりは絶対少ない
int first_coord;  // 高橋くんの初期位置:(y,x)はy*W+xとして表現する
int tiles[H * W];  // tiles[coord1]とtiles[coord2]が同じなら同じタイル
int points[H * W];  // tiles[coord]を訪れた場合の得点
vector<vector<int>> next_coords{H * W};  // next_coords[coord]=coordの隣のリスト

// 始点を指定して探索するクラス
class DfsSolver {
 public:
  // visiteds_[tile]はtileが踏まれたかどうかを表す
  array<bool, TILE_N> visiteds_;
  vector<int> path_;       // coordのリスト。今探索中のパス
  vector<int> best_path_;  // coordのリスト。今のところ一番いいパス
  int best_score_ = 0;     // 今のところ一番いいスコア
  int score_ = 0;          // 今探索中のパスのスコア
  int remaining_search_cnt_;  // 残り探索回数

  // dfsを開始する。dfsは再帰で実装するので開始する関数が必要
  void start() {
    best_path_.clear();
    score_ = 0;
    visiteds_.fill(0);
    path_.clear();
    remaining_search_cnt_ = 4000;
    dfs(first_coord);
  }

 private:
  // coordを始点とした探索
  void dfs(const int coord) {
    path_.emplace_back(coord);
    score_ += points[coord];
    visiteds_[tiles[coord]] = 1;

    if (best_score_ < score_) {
      best_score_ = score_;
      best_path_ = path_;
    }
    remaining_search_cnt_--;
    if (remaining_search_cnt_ == 0) return;
    vector<int> legal_next_coords;
    for (const auto &next_coord : next_coords[coord]) {
      if (!visiteds_[tiles[next_coord]]) {
        legal_next_coords.emplace_back(next_coord);
      }
    }
    for (const auto &next_coord : legal_next_coords) {
      if (!visiteds_[tiles[next_coord]]) {
        dfs(next_coord);
        if (remaining_search_cnt_ == 0) return;
      }
    }
    path_.pop_back();
    score_ -= points[coord];
    visiteds_[tiles[coord]] = 0;
  }
};

// 始点と終点を指定して問題の一部を探索するクラス
// 近傍計算に使う
class DfsPartSolver {
 public:
  // visiteds_[tile]はtileが踏まれたかどうかを表す
  array<bool, TILE_N> visiteds_;
  vector<int> path_;       // coordのリスト。今探索中のパス
  vector<int> best_path_;  // coordのリスト。今のところ一番いいパス
  int best_score_ = 0;     // 今のところ一番いいスコア
  int score_ = 0;          // 今探索中のパスのスコア
  int remaining_search_cnt_;  // 残り探索回数
  int goal_ = 0;              // 目標地点

  // dfsを開始する。dfsは再帰で実装するので開始する関数が必要
  // DfsSolverのstartとは以下の点で異なる
  //  始点、終点を指定する
  //  注目しない部分はタイルが踏まれているものとして初期化する
  void start(const int start, const int goal,
             const array<bool, TILE_N> &visiteds,
             const int remaining_search_cnt) {
    goal_ = goal;
    best_path_.clear();
    best_score_ = 0;
    score_ = 0;
    visiteds_ = visiteds;
    path_.clear();
    remaining_search_cnt_ = remaining_search_cnt;
    dfs(start);
  }

  // coordを始点としてgoal_を終点とした探索
  void dfs(const int coord) {
    if (!visiteds_[tiles[coord]]) {
      path_.emplace_back(coord);
      score_ += points[coord];
      visiteds_[tiles[coord]] = 1;
    }
    remaining_search_cnt_--;
    if (remaining_search_cnt_ == 0) return;
    if (coord != goal_) {
      vector<int> legal_next_coords;
      for (const auto &next_coord : next_coords[coord]) {
        if (!visiteds_[tiles[next_coord]]) {
          legal_next_coords.emplace_back(next_coord);
        } else if (next_coord == goal_) {
          // goalにたどり着いた時点で探索を終了
          best_score_ = score_;
          best_path_ = path_;
          remaining_search_cnt_ = 0;
          return;
        }
      }
      shuffle(legal_next_coords.begin(), legal_next_coords.end(), rnd.mt_);
      for (const auto &next_coord : legal_next_coords) {
        if (!visiteds_[tiles[next_coord]]) {
          dfs(next_coord);
          if (remaining_search_cnt_ == 0) return;
        }
      }
    }

    path_.pop_back();
    score_ -= points[coord];
    visiteds_[tiles[coord]] = 0;
  }
};

struct State {
  vector<int> now_path_;  // coordのリスト。今までに遷移してきた現在のパス

  // 初期解を生成する
  void solve_first() {
    DfsSolver dfs_solver{};
    dfs_solver.start();
    this->now_path_ = dfs_solver.path_;
  }

  void simulatedAnnealingWithTimeThreshold(const int64_t time_threshold,
                                           const double start_temp,
                                           const double end_temp) {
    /***************************************************************/
    // 1. 時間計測開始
    auto time_keeper = TimeKeeperDouble(time_threshold);
    // end of 1
    /***************************************************************/

    /***************************************************************/
    // 2. 初期解を生成し、ベストスコアを更新する。
    //    ループ前に必要な変数があれば用意する。
    //    初期解生成が重すぎる場合は時間計測を分ける必要あり。
    this->solve_first();

    array<bool, TILE_N> now_visiteds{};
    DfsPartSolver dfs_part_solver{};
    for (int i = 0; i < now_path_.size(); i++) {
      now_visiteds[tiles[now_path_[i]]] = 1;
    }
    // end of 2
    /***************************************************************/

    for (;;) {
      /*************************************************************/
      // 3. 制限時間の確認
      time_keeper.setNowTime();
      if (time_keeper.isTimeOver()) {
        break;
      }
      // end of 3
      /*************************************************************/

      /*************************************************************/
      // 4. 焼きなましの温度と遷移の閾値を決定する。
      //    以下の例ではstart_tempから経過時間に応じて線形にend_tempに冷却する。
      //    冷却スケジュールは線形である必要はない。
      double temp =
          start_temp +
          (end_temp - start_temp) * (time_keeper.getNowTime() / time_threshold);
      // end of 4
      /************************************************************/

      /*************************************************************/
      // 5. 近傍から1つ選んでスコアを計算

      // 今のパスの中で壊す部分パスの長さ。
      // コンテスト1位のatsさんは徐々に狭くしていたが、しなくてもいいスコアになる。
      int delete_path_length = rnd.nextInt((int)(1 + now_path_.size() * 0.05));
      int start_path_id = rnd.nextInt(now_path_.size() - delete_path_length);
      int end_path_id = start_path_id + delete_path_length;

      auto next_visiteds = now_visiteds;
      // 探索するパスの長さよりちょっと多いぐらいの探索をする
      int remaining_search_cnt = 4 * delete_path_length;

      // start_path_idとend_path_idの間のパスを削り、
      // 削った部分のスコアをメモする
      int now_score = 0;
      for (int i = start_path_id + 1; i <= end_path_id - 1; i++) {
        now_score += points[now_path_[i]];
        next_visiteds[tiles[now_path_[i]]] = 0;
      }
      dfs_part_solver.start(now_path_[start_path_id], now_path_[end_path_id],
                            next_visiteds, remaining_search_cnt);
      // end of 5
      /*************************************************************/

      /*************************************************************/
      // 6. 試した近傍を反映させる（遷移する）
      //    ※実装方針次第では5の段階で反映させ、
      //      6では遷移させたくない場合に元の状態に戻す方針もできる

      int next_score = dfs_part_solver.best_score_;
      int diff = next_score - now_score;
      if (dfs_part_solver.best_path_.size() > 0 &&
          exp(diff / temp) > rnd.nextDouble()) {
        // visitedsの更新
        now_visiteds = dfs_part_solver.visiteds_;

        // 今回生成した部分パスを挿入した全体パスを生成
        vector<int> transitioned_path;
        for (int i = 0; i <= start_path_id; i++) {
          transitioned_path.emplace_back(now_path_[i]);
        }
        for (int i = 0; i < dfs_part_solver.best_path_.size(); i++) {
          transitioned_path.emplace_back(dfs_part_solver.best_path_[i]);
        }
        for (int i = end_path_id; i < now_path_.size(); i++) {
          transitioned_path.emplace_back(now_path_[i]);
        }
        now_path_ = transitioned_path;
      }
      // end of 6
      /*************************************************************/
    }
  }

  // 入力を受け取る
  void input() {
    int sy, sx;
    cin >> sy >> sx;
    first_coord = sy * W + sx;
    for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
        cin >> tiles[y * W + x];
      }
    }
    for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
        cin >> points[y * W + x];
      }
    }
  }

  // 解を出力する
  void output() {
    // now_pathは座標のリストになっているため、移動方向に直して出力する
    string res;
    for (int i = 0; i < now_path_.size() - 1; i++) {
      int diff = now_path_[i + 1] - now_path_[i];
      if (diff == 1) {
        res.push_back('R');
      } else if (diff == -1) {
        res.push_back('L');
      } else if (diff == W) {
        res.push_back('D');
      } else if (diff == -W) {
        res.push_back('U');
      }
    }
    cout << res << endl;
  }
};

void initialize_global_info() {
  // 各座標から行ける場所(盤面の枠内かつタイルが異なる)をメモしておく
  const int dy[4] = {1, 0, -1, 0};
  const int dx[4] = {0, 1, 0, -1};
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      int coord = y * W + x;
      for (int d = 0; d < 4; d++) {
        int ty = y + dy[d];
        int tx = x + dx[d];
        if (ty >= 0 && ty < H && tx >= 0 && tx < W) {
          int tcoord = ty * W + tx;
          if (tiles[coord] != tiles[tcoord]) {
            next_coords[coord].emplace_back(tcoord);
          }
        }
      }
    }
  }
}

int main() {
  State state;
  // 入力を受け取る
  state.input();
  // 前計算などの準備
  initialize_global_info();
  // 焼きなまし法で解を求める
  state.simulatedAnnealingWithTimeThreshold(/*time_threshold*/ 1950,
                                            /*start_temp*/ 150,
                                            /*end_temp*/ 0);
  // 解を出力する
  state.output();
  return 0;
}