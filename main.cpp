#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
using namespace std;

class Solver{
    public: 
        int starti;
        int startj;
        int tileMap[50][50];
        int scoreMap[50][50];
        int maxStep = -1;
        int maxTileNum = -1;
        string ans;
        int direction[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        char next[4] = {'D', 'U', 'R', 'L'};

        void Start(){
            GetInput();
            vector<bool> temp(maxTileNum + 1, false);
            DFS(starti, startj, temp, 0, "");
        }

        void GetInput(){
            cin >> starti >> startj;
            for (int i=0; i<50; i++){
                for (int j=0; j<50; j++){
                    cin >> tileMap[i][j];
                    tileMap[i][j]--;
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
            bool flag = true;
            
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
                usedTile[tileNum]=true;
                DFS(tate, yoko, usedTile, step+1, route+next[i]);
                usedTile[tileNum]=false;
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
    Solver solver;
    solver.Start();
    return 0;
}