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

unsigned px = MAPHEIGHT-1;
unsigned py = MAPLENGHT/2;

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
    printf("       (a/d - Move, w - Jump, Double Jump, i/j/k/l - Attack) \n");

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
            }else{
                map[i][j] = ' ';
            }
    }
}
}

void GenerateMap(){
    for(int i = 0; i < MAPHEIGHT; i++){
        for(int j = 0; j < MAPLENGHT; j++){
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

void Movement(char input){
    int NewX = px;
    int NewY = py;

    if(input == 'w'){
        NewX -= 2;
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
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {(short)py, (short)px});
    cout << " ";
    px = NewX;
    py = NewY;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {(short)py, (short)px});
    map[px][py] = '@';
    cout << "@";
}
int main() {
    GenerateUi();
    FillMap();
    GenerateMap();
    while(true){
        char key = _getch();
        Movement(key);
    }
    return 0;
}