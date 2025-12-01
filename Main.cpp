#include <iostream>
#include<ctime>
#include<conio.h>
#include <cstdlib>
#include <windows.h>

using::std::cout;
using::std::endl;
using::std::cin;

const int MAX_JUMPS = 2;
const int UI_HEIGHT = 1;
const int MAPHEIGHT = 30;
const int MAPLENGHT = 80;
const int GRAVITY_DELAY = 300;

char map[MAPHEIGHT][MAPLENGHT];

unsigned long lastGravity = 0;
int jumpCount = 0;
int px = MAPHEIGHT-2;
int py = MAPLENGHT/2;


void StartScreen(){
    cout << "Welcome to the Game!" << endl;
    cout << "Type start to begin!" << endl;
    char input[10];
    cin >> input;
    if(strcmp(input, "start") == 0){
        return;
    }else{
        system("cls");
        StartScreen();
    }
}

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
            cout << map[i][j];
        }
        cout << endl;
    }
}

void Movement(char input){
    int NewX = px;
    int NewY = py;

    if(input == 'w' && jumpCount < MAX_JUMPS){
        NewX -= 2;
        jumpCount++;
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
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {(short)py, (short)(px+UI_HEIGHT)});
    cout << " ";
    
    px = NewX;
    py = NewY;
    
    map[px][py] = '@';
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {(short)py, (short)(px+UI_HEIGHT)});
    cout << "@";
}
int main() {
    StartScreen();
    system("cls");
    GenerateUi();
    FillMap();
    GenerateMap();
    while(true){
        char key = 0;
        if(_kbhit()){
            key = _getch();
            Movement(key);
        }

        unsigned long now = clock();
        if(map[px+1][py] == ' ' && now - lastGravity > GRAVITY_DELAY){
        Movement('s');
        lastGravity = now;
        } else if(map[px+1][py] != ' '){
        jumpCount = 0;  // landed
        }

        Sleep(1000/30);
    }
    return 0;
}