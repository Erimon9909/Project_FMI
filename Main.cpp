#include <iostream>
#include<ctime>
#include<conio.h>
#include <cstdlib>
#include <windows.h>

using::std::cout;
using::std::endl;
using::std::cin;

const int MAPHEIGHT = 30;
const int MAPLENGHT = 80;
char map[MAPHEIGHT][MAPLENGHT];

int px = MAPHEIGHT-2;
int py = MAPLENGHT/2;

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

void GeneratePlayer(){
    char player = '@';
    map[px][py] = player;
}

void FillMap(){
    for(int i = 0; i < MAPHEIGHT; i++){
        for(int j = 0; j < MAPLENGHT; j++){
            if(i == 0 || i == MAPHEIGHT-1 || j == 0 || j == MAPLENGHT-1){
                map[i][j] = '#';
            }else if(i == px && j == py){
                GeneratePlayer();
            }else{
                map[i][j] = ' ';
            }
    }
}
}

void GenerateMap(){
    for(int i = 0; i < MAPHEIGHT; i++){
        for(int j = 0; j < MAPLENGHT; j++){
            cout << map[i][j];
        }
        cout << endl;
    }
}

void Movement(char input){
    int NewX = px;
    int NewY = py;

    if(input == 'w'){
        NewX--;
    }else if(input == 's'){
        NewX++;
    }else if(input == 'a'){
        NewY--;
    }else if(input == 'd'){
        NewY++;
    }
    //Collision Detection
    if(map[NewX][NewY] == '#'){
        return;
    }
    //Move Player
    map[px][py] = ' ';
    px = NewX;
    py = NewY;
    map[px][py] = '@';
}
int main() {
    GenerateUi();
    cout << endl;
    FillMap();
    GenerateMap();
    while(true){
        char key = _getch();
        Movement(key);
        system("cls");
        GenerateUi();
        cout << endl;
        FillMap();
        GenerateMap();
    }
    return 0;
}