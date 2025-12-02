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
const int MAPHEIGHT = 20;
const int MAPLENGHT = 95;
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

void DrawAt(int x, int y, char c){
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {(short)y, (short)(x+UI_HEIGHT)});
    cout << c;
}

void GeneratePlatform(int row, int startCol, int length){
    if(row <= 0 || row >= MAPHEIGHT-1) return;
    if(startCol < 1) startCol = 1;
    if(startCol + length >= MAPLENGHT - 1)
        length = (MAPLENGHT - 2) - startCol;

    for(int c = startCol; c < startCol + length; c++){
        map[row][c] = '=';
    }
}

void GeneratePlatforms() {
    const int minLen = 5;
    const int maxLen = 15;

    // Fixed rows
    int rows[5] = {17, 15, 6, 5, 4}; // bottom to top

    // Horizontal start for each platform
    int xStarts[5];

    // First platform: bottom, anywhere near left third
    xStarts[0] = rand() % (MAPLENGHT / 3 - maxLen) + 1;
    int length0 = rand() % (maxLen - minLen + 1) + minLen;

    for (int x = 0; x < length0; x++)
        map[rows[0]][xStarts[0] + x] = '=';

    // Second platform: within jump reach horizontally
    int length1 = rand() % (maxLen - minLen + 1) + minLen;
    int minX1 = xStarts[0] - 3;
    int maxX1 = xStarts[0] + length0 + 3;
    if (minX1 < 1) minX1 = 1;
    if (maxX1 + length1 >= MAPLENGHT - 1) maxX1 = MAPLENGHT - length1 - 2;
    xStarts[1] = rand() % (maxX1 - minX1 + 1) + minX1;

    for (int x = 0; x < length1; x++)
        map[rows[1]][xStarts[1] + x] = '=';

    // Top 3 platforms: spread across map width (to left, center, right)
    int topLengths[3];
    for (int i = 0; i < 3; i++) {
        topLengths[i] = rand() % (maxLen - minLen + 1) + minLen;
    }

    // Distribute across map
    xStarts[2] = rand() % ((MAPLENGHT/3) - topLengths[0]) + 1;                    // left
    xStarts[3] = rand() % ((MAPLENGHT/3) - topLengths[1]) + (MAPLENGHT/3);       // middle
    xStarts[4] = rand() % ((MAPLENGHT/3) - topLengths[2]) + (2*MAPLENGHT/3);     // right

    for (int i = 2; i < 5; i++) {
        for (int x = 0; x < topLengths[i-2]; x++)
            map[rows[i]][xStarts[i] + x] = '=';
    }
}


void GeneratePlayer(){
    char player = '@';
    map[px][py] = player;
}

void FillMap(){
    // Clear map
    for(int i = 0; i < MAPHEIGHT; i++){
        for(int j = 0; j < MAPLENGHT; j++){
            map[i][j] = ' ';
        }
    }

    // Borders
    for(int i = 0; i < MAPHEIGHT; i++){
        map[i][0] = '#';
        map[i][MAPLENGHT-1] = '#';
    }
    for(int j = 0; j < MAPLENGHT; j++){
        map[0][j] = '#';
        map[MAPHEIGHT-1][j] = '#';
    }

    // Generate reachable platforms
    GeneratePlatforms();

    // Place player
    map[px][py] = '@';
}

void GenerateMap(){
    for(int i = 0; i < MAPHEIGHT; i++){
        for(int j = 0; j < MAPLENGHT; j++){
            cout << map[i][j];
        }
        cout << endl;
    }
}

void Attack(char input){
    switch(input){
        case 'i':
            if(map[px-1][py] == '#' || map[px-1][py] == '='){
                break;
            }
            DrawAt(px-1, py-1, '/');
            DrawAt(px-1, py, '-');
            DrawAt(px-1, py+1, '\\');

            Sleep(500);
            DrawAt(px-1, py-1, ' ');
            DrawAt(px-1, py, ' ');
            DrawAt(px-1, py+1, ' ');
            break;
        case 'k':
            if(map[px+1][py] == '#' || map[px+1][py] == '='){
                break;
            }
            DrawAt(px+1, py-1, '\\');
            DrawAt(px+1, py, '-');
            DrawAt(px+1, py+1, '/');

            Sleep(500);
            DrawAt(px+1, py-1, ' ');
            DrawAt(px+1, py, ' ');
            DrawAt(px+1, py+1, ' ');
            break;
        case 'j':
            if(map[px+1][py-1] == '#' || map[px+1][py-1] == '='){
                break;
            }
            DrawAt(px-1, py-1, '/');
            DrawAt(px, py-1, '|');
            DrawAt(px+1, py-1, '\\');

            Sleep(500);
            DrawAt(px-1, py-1, ' ');
            DrawAt(px, py-1, ' ');
            DrawAt(px+1, py-1, ' ');
            break;
        case 'l':
            if(map[px+1][py+1] == '#' || map[px+1][py+1] == '='){
                break;
            }
            DrawAt(px-1, py+1, '\\');
            DrawAt(px, py+1, '|');
            DrawAt(px+1, py+1, '/');

            Sleep(500);
            DrawAt(px-1, py+1, ' ');
            DrawAt(px, py+1, ' ');
            DrawAt(px+1, py+1, ' ');
            break;
    }
}
void Movement(char input){
    int NewX = px;
    int NewY = py;

    switch(input){
        case 'a':
            NewY--;
            break;
        case 'd':
            NewY++;
            break;
        case 'w':
            if(jumpCount < MAX_JUMPS){
                NewX -= 2;
                jumpCount++;
            }
            break;
        case 's':
            NewX++;
            break;
        case 'q':
            if(jumpCount < MAX_JUMPS){
                NewX -= 2;
                NewY --;
                jumpCount++;
            }
            break;
        case 'e':
            if(jumpCount < MAX_JUMPS){
                NewX -= 2;
                NewY ++;
                jumpCount++;
            }
            break;
    }
    //Collision Detection
    if(map[NewX][NewY] == '#' || map[NewX][NewY] == '='){
        return;
    }
    //Move Player
    
    map[px][py] = ' ';  
    DrawAt(px, py, ' ');
    
    px = NewX;
    py = NewY;
    
    map[px][py] = '@';
    DrawAt(px, py, '@');
}
int main() {
    StartScreen();
    system("cls");
    GenerateUi();
    FillMap();
    GeneratePlatforms();
    GenerateMap();
    while(true){
        char key = 0;
        if(_kbhit()){
            key = _getch();
            Movement(key);
            Attack(key);
        }

        unsigned long now = clock();
        if(map[px+1][py] == ' ' && now - lastGravity > GRAVITY_DELAY){
        Movement('s');
        lastGravity = now;
        } else if(map[px+1][py] != ' '){
        jumpCount = 0;  // landed
        }

        Sleep(1000/60);
    }
    return 0;
}