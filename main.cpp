#include <iostream>
using namespace std;

class Solver{
    public: 
        int starti;
        int startj;
        int tileMap[50][50];
        int scoreMap[50][50];

        void Start(){
            GetInput();
            DFS();
        }

        void GetInput(){
            cin >> starti >> startj;
            for (int i=0; i<50; i++){
                for (int j=0; j<50; j++){
                    cin >> tileMap[i][j];
                }
            }
            for (int i=0; i<50; i++){
                for (int j=0; j<50; j++){
                    cin >> scoreMap[i][j];
                }
            }
        }
};



int main(){
    Solver solver;
    solver.Start();
    return 0;
}