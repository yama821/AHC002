#include <stdio.h>

class main{
    int starti;
    int startj;
    int[][] tileMap;
    int[][] scoreMap;

    int main(){
        Start();
        return 0;
    }

    void Start(){
        GetInput();
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
}