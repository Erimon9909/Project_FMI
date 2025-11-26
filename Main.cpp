#include <iostream>

using::std::cout;
using::std::endl;
using::std::cin;
class Player{
    public:
    int HP = 5;
    char Sprite = '@';
    
};
void GenerateUi(){
    int HP = 5;
    cout << "Player HP: ";
        for(int i = 0; i < HP; i++){
            if(i == 4){
                cout << "O";
            }else{
            cout << "O-";
            }
        }
    cout << "       (a/d - Move, w - Jump, Double Jump, i/j/k/l - Attack)";

}

void GenerateMap(){
    int mapHeight = 40;
    int mapLenght = 80;
    char map[mapHeight][mapLenght];
    for(int i = 0; i < mapHeight; i++){
        for(int j = 0; j < mapLenght; j++){
            if(i == 0 || i == mapHeight-1 || j == 0 || j == mapLenght-1){
                map[i][j] = '#';
                cout << map[i][j]; 
            }else{
                map[i][j] = ' ';
                cout << map[i][j];
            }
        }
        cout << endl;
    }
}
int main() {
    GenerateUi();
    cout << endl;
    GenerateMap();
    return 0;
}