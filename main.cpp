#include <bits/stdc++.h>
using namespace std;

const int N = 50;

/*
    0 -> U, 1 -> D, 2 -> L, 3 -> R
*/
string opes = "UDLR";
int dx[4] = {-1, 1, 0, 0}, dy[4] = {0, 0, -1, 1};

struct Tiles {
    int si, sj;
    int t[50][50], p[50][50];

    Tiles() {}
    
    void input() {
        cin >> si >> sj;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) cin >> t[i][j];
        }
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) cin >> p[i][j];
        }
    }
};

struct State {
    Tiles tiles;
    set<int> visited;
    string history;
    int score;
    int x, y;

    State() {}
    
    State(Tiles _tiles): tiles(_tiles), score(0) {
        x = tiles.si;
        y = tiles.sj;
        visited.insert(tiles.t[x][y]);
        score += tiles.p[x][y];
    }

    // opes[d] 方向への移動が可能か判定
    bool isAccepted(int d) {
        int nx = x + dx[d], ny = y + dy[d];
        
        if (nx < 0 || N <= nx || ny < 0 || N <= ny) return false;

        if (visited.find(tiles.t[nx][ny]) != visited.end()) return false;

        return true;
    }

    // 可能な操作と得られるスコアの組を返す
    vector<pair<int, int>> acceptedActions() {
        vector<pair<int, int>> ret;
        for (int d = 0; d < 4; d++) {
            if (isAccepted(d)) {
                int plus = tiles.p[x + dx[d]][y + dy[d]];
                ret.push_back({d, plus});
            }
        }
        return ret;
    }

    // opes[d] 方向に移動
    void advance(int d) {
        assert(isAccepted(d));
        history.push_back(opes[d]);
        x += dx[d];
        y += dy[d];
        score += tiles.p[x][y];
        visited.insert(tiles.t[x][y]);
    }

    bool operator<(const State& other) const {
        return score < other.score;
    }

    bool operator>(const State& other) const {
        return score > other.score;
    }

    bool isDone() {
        auto vec = acceptedActions();
        return vec.empty();
    }
};

struct Solver {
    State state;

    Solver() {
        Tiles tiles;
        tiles.input();
        state = State(tiles);
    }


    void solve() {
        while (1) {
            auto actions = state.acceptedActions();
            if (actions.empty()) break;

            priority_queue<State> pq;
            for (auto action: actions) {
                auto nextstate = state;
                nextstate.advance(action.first);
                pq.push(nextstate);
            }

            state = pq.top();
        }

        cout << state.history << endl;
    }

};

int main() {
    Solver solver;
    solver.solve();
}