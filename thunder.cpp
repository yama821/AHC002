// DFSのサンプルコード

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

struct State {
  vector<int> now_path_;  // coordのリスト。今までに遷移してきた現在のパス

  // 初期解を生成する
  void solve_first() {
    DfsSolver dfs_solver{};
    dfs_solver.start();
    this->now_path_ = dfs_solver.path_;
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
  // DFSで解を求める
  state.solve_first();
  // 解を出力する
  state.output();
  return 0;
}