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
        1e-3; // ms
  }

  // 経過時間をnow_time_に取得する。
  double getNowTime() const { return this->now_time_; }

  // インスタンス生成した時から指定した時間制限を超過したか判定する。
  bool isTimeOver() const { return now_time_ >= time_threshold_; }
};

class Solver {
    public: 
        TimeKeeperDouble time_keeper;
        int starti;
        int startj;
        int tileMap[50][50];
        int scoreMap[50][50];
        int maxStep = -1;
        int maxTileNum = -1;
        int startTile = -1;
        string ans;
        int direction[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        char next[4] = {'D', 'U', 'R', 'L'};
        
        Solver(double time_threshold) : time_keeper(time_threshold) {}

        void Start(){
            GetInput();
            Process();
            GetOutput();
        }
        
        void Process(){
          vector<bool> temp(maxTileNum + 1, false);
          temp[startTile]=true;
          string s = "";
          DFS(starti, startj, temp, 0, s);
        }
        
        void GetOutput(){
            cout << ans << endl;
        }

        void GetInput(){
            cin >> starti >> startj;
            for (int i=0; i<50; i++){
                for (int j=0; j<50; j++){
                    cin >> tileMap[i][j];
                    if (i==starti && j==startj){
                      startTile = tileMap[i][j];
                    }
                    if (tileMap[i][j]>maxTileNum){
                        maxTileNum=tileMap[i][j];
                    }
                }
            }
            for (int i=0; i<50; i++){
                for (int j=0; j<50; j++){
                    cin >> scoreMap[i][j];
                }
            }
        }

        void DFS(int nowi, int nowj, vector<bool> usedTile, int step, string route){
          
            time_keeper.setNowTime();
            if (time_keeper.isTimeOver()) {
                return;
            }
            
            bool flag = true;
            //cout << route << "dfs" << step << endl;
            
            for (int i=0; i<4; i++){
                int tate = nowi+direction[i][0];
                int yoko = nowj+direction[i][1];
                if (tate<0 || tate>49 || yoko<0 || yoko>49){
                    continue;
                }
                int tileNum = tileMap[tate][yoko];
                if (usedTile[tileNum]){
                    continue;
                }
                flag = false;
                vector<bool> temp (usedTile.size());
                copy (usedTile.begin(), usedTile.end(), temp.begin());
                temp[tileNum]=true;
                DFS(tate, yoko, temp, step+1, route+next[i]);
            }

            if (flag){
                if (step>maxStep){
                    ans = route;
                    maxStep = step;
                }
            }        
        }
};



int main(){
    Solver solver (1500);
    solver.Start();
    return 0;
}