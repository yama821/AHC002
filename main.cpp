#include <bits/stdc++.h>
using namespace std;

struct Tiles {
    int n;
    int si, sj;
    int t[50][50], p[50][50];

    Tiles():n(50) {
        cin >> si >> sj;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) cin >> t[i][j];
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) cin >> p[i][j];
        }
    }
};

struct State {
    Tiles tiles;
    bool visited[2500];
    int x, y;

    State() {}
    
    State(Tiles _tiles): tiles(_tiles) {
        for (int i = 0; i < 2500; i++) visited[i] = 0;
        x = tiles.si;
        y = tiles.sj;
    }

    


};

struct Solver {
    State state;

    Solver() {
        Tiles tiles;
        state = State(tiles);
    }


};

int main() {

}