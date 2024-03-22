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
    int first_action = -1;

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
    vector<pair<int, int>> legalActions() {
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
        auto vec = legalActions();
        return vec.empty();
    }
};

struct Solver {
    State state;
    int beam_width = 10;
    int beam_depth = 100;

    Solver() {
        Tiles tiles;
        tiles.input();
        state = State(tiles);
    }

    int beamSearchAction() {
        priority_queue<State> now_beam;
        State best_state;

        now_beam.push(state);
        for (int t = 0; t < beam_depth; t++) {
            priority_queue<State> next_beam;
            for (int i = 0; i < beam_width; i++) {
                if (now_beam.empty()) break;
                State now_state = now_beam.top();
                now_beam.pop();

                auto legal_actions = now_state.legalActions();
                if (legal_actions.empty()) next_beam.push(now_state);

                for (auto &action: legal_actions) {
                    State next_state = now_state;
                    next_state.advance(action.first);
                    if (t == 0) next_state.first_action = action.first;
                    next_beam.push(next_state);
                }
            }

            now_beam = next_beam;
            best_state = now_beam.top();

            if (best_state.isDone()) {
                // cout << "!!!!" << endl;
                cout << t << " " << best_state.score << endl;
                break;
            }
        }
        return best_state.first_action;
    }

    int greedyAction() {
        auto legal_actions = state.legalActions();
        int best_score = 0, best_action = -1;
        for (auto action: legal_actions) {
            auto next_state = state;
            next_state.advance(action.first);
            if (best_score < next_state.score) {
                best_score = next_state.score;
                best_action = action.first;
            }
        }
        return best_action;
    }

    void solve() {
        while (!state.isDone()) {
            int action = beamSearchAction();
            state.advance(action);
        }
        
        cout << state.history << endl;
    }

};

int main() {
    Solver solver;
    solver.solve();
}