#include <iostream>
#include <ctime>
#include <conio.h>
#include <cstdlib>
#include <windows.h>
#include <vector>

using std::cout;
using std::endl;

const int MAX_JUMPS = 2;
const int UI_HEIGHT = 1;
const int MAPHEIGHT = 20;
const int MAPLENGHT = 95;
const double ENEMY_MOVE_DELAY = 0.2;

const int TARGET_FPS = 60;
const double FRAME_TIME = 1.0 / TARGET_FPS;

char map[MAPHEIGHT][MAPLENGHT];

clock_t lastFrameTime;
unsigned long lastGravity = 0;

int jumpCount = 0;
int currentWave = 1;
bool waveActive = false;

const int MAX_WAVES = 5;
// ================= PLAYER =================
struct player {
    int x = MAPHEIGHT - 2;
    int y = MAPLENGHT / 2;
    char icon = '@';
};
player p;

// ================= ENEMY =================
enum EnemyType{
    WALKER,
    FLYER,
    CRAWLER,
    JUMPER,
    BOSS
};

struct enemy {
    int x;
    int y;
    int hp;
    EnemyType type;
    char icon;
    double timer;
};
std::vector<enemy> enemies;
// ================= UTILS =================
void drawAt(int x, int y, char c) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
        { (short)y, (short)(x + UI_HEIGHT) });
    cout << c;
}

int randRange(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// ================= UI =================
void generateUi() {
    int HP = 5;
    cout << "Player HP: ";
    for (int i = 0; i < HP; i++) {
        cout << "O";
        if (i != HP - 1) cout << "-";
    }
    cout << "   (a/d Move, w Jump, i/j/k/l Attack)\n";
}

// ================= MAP =================
void generatePlatforms() {
    int minLength = 10, maxLength = 15;
    int platformCount = 7;
    int platformHeight = MAPHEIGHT - 4;
    int platformGap = 4;

    for (int i = 0; i < platformCount; i++) {
        int length = rand() % (maxLength - minLength + 1) + minLength;
        for (int j = 0; j < length; j++)
            map[platformHeight][j + platformGap] = '=';

        platformHeight -= 3;
        platformGap += (i == 3 ? -20 : (i > 3 ? 21 : 8));
        if (platformHeight < 3) platformHeight += 4;
    }
}

void generateMap() {
    for (int i = 0; i < MAPHEIGHT; i++) {
        for (int j = 0; j < MAPLENGHT; j++) {
            if (i == 0 || i == MAPHEIGHT - 1 || j == 0 || j == MAPLENGHT - 1)
                map[i][j] = '#';
            else
                map[i][j] = ' ';
        }
    }
    generatePlatforms();
    map[p.x][p.y] = p.icon;
}

void printMap() {
    generateMap();
    for (int i = 0; i < MAPHEIGHT; i++) {
        for (int j = 0; j < MAPLENGHT; j++)
            cout << map[i][j];
        cout << endl;
    }
}

// ================= MOVEMENT =================
void movement(char input) {
    int NewX = p.x;
    int NewY = p.y;

    if (input == 'a') NewY--;
    if (input == 'd') NewY++;
    if (input == 'w' && jumpCount < MAX_JUMPS) {
        NewX -= 2;
        jumpCount++;
    }
    if (input == 's') NewX++;

    if (map[NewX][NewY] != ' ')
        return;

    drawAt(p.x, p.y, ' ');
    map[p.x][p.y] = ' ';

    p.x = NewX;
    p.y = NewY;

    map[p.x][p.y] = '@';
    drawAt(p.x, p.y, '@');
}

// ================= ATTACK SYSTEM =================
bool attacking = false;
double attackTimer = 0;
char attackDir;

void drawAttack(char d) {
    if (d == 'i') {
        if(map[p.x - 1][p.y - 1] != ' ' || map[p.x - 1][p.y] != ' ' || map[p.x - 1][p.y + 1] != ' ')
            return;
        drawAt(p.x - 1, p.y - 1, '/');
        drawAt(p.x - 1, p.y, '-');
        drawAt(p.x - 1, p.y + 1, '\\');
    }
    if (d == 'k') {
        if(map[p.x + 1][p.y - 1] != ' ' || map[p.x + 1][p.y] != ' ' || map[p.x + 1][p.y + 1] != ' ')
            return;
        drawAt(p.x + 1, p.y - 1, '\\');
        drawAt(p.x + 1, p.y, '_');
        drawAt(p.x + 1, p.y + 1, '/');
    }
    if (d == 'j') {
        if(map[p.x - 1][p.y - 1] != ' ' || map[p.x][p.y - 1] != ' ' || map[p.x + 1][p.y - 1] != ' ')
            return;
        drawAt(p.x - 1, p.y - 1, '/');
        drawAt(p.x, p.y - 1, '|');
        drawAt(p.x + 1, p.y - 1, '\\');
    }
    if (d == 'l') {
        if(map[p.x - 1][p.y + 1] != ' ' || map[p.x][p.y + 1] != ' ' || map[p.x + 1][p.y + 1] != ' ')
            return;
        drawAt(p.x - 1, p.y + 1, '\\');
        drawAt(p.x, p.y + 1, '|');
        drawAt(p.x + 1, p.y + 1, '/');
    }
}

void clearAttack(char d) {
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
            drawAt(p.x + i, p.y + j, map[p.x + i][p.y + j]);
}

void attack(char input) {
    if (attacking) return;
    if (input == 'i' || input == 'j' || input == 'k' || input == 'l') {
        attacking = true;
        attackDir = input;
        attackTimer = 0;
        drawAttack(input);
    }
}

void updateAttack(double dt) {
    if (!attacking) return;
    attackTimer += dt;
    if (attackTimer >= 0.2) {
        clearAttack(attackDir);
        attacking = false;
    }
}

// ================= ENEMIES (HOOK) =================
void spawnEnemy(int x, int y, EnemyType type) {
    enemy e;
    e.x = x;
    e.y = y;
    e.type = type;
    e.hp = 1; // default hp
    e.timer = 0;

    switch (type) {
    case WALKER:
        e.icon = 'W';
        break;
    case FLYER:
        e.icon = 'F';
        break;
    case CRAWLER:
        e.icon = 'C';
        break;
    case JUMPER:
        e.icon = 'J';
        break;
    case BOSS:
        e.icon = 'B';
        e.hp = 3; // boss has more hp
        break;
    }

    enemies.push_back(e);
    drawAt(e.x, e.y, e.icon);
}

void spawnRandomEnemy(EnemyType maxType){
    EnemyType t = (EnemyType)randRange(0, maxType);

    int x,y;
    do{
        x = randRange(1, MAPHEIGHT -2);
        y = randRange(1, MAPLENGHT -2);
    }while (map[x][y] != ' ' || map[x+1][y] == ' ');

    spawnEnemy(x, y, t);
}

bool isEmpty(int x, int y) {
    return map[x][y] == ' ';
}

void moveEnemy(enemy &e, int newX, int newY) {
    if (!isEmpty(newX, newY))
        return;

    drawAt(e.x, e.y, ' ');
    e.x = newX;
    e.y = newY;
    drawAt(e.x, e.y, e.icon);
}

void updateWalker(enemy &e){
    int dir = (p.y < e.y) ? -1 : 1;

    if(map[e.x +1][e.y] == ' '){
        moveEnemy(e, e.x +1, e.y);
        return;
    }

    if(map[e.x + 1][e.y + dir] != '#' && map[e.x][e.y + dir] != '='){
        return;
    }

    moveEnemy(e, 0, dir);
}

void updateFlyer(enemy& e, double dt){
    e.timer += dt;

    if(e.timer > 2.0){
        e.timer = 0;
        if(p.x > e.x && isEmpty(e.x +1, e.y)){
            moveEnemy(e, 1, 0);
        }
    }
}
bool isWall(int x, int y){
    return map[x][y] == '#' || map[x][y] == '=';
}

void updateCrawler(enemy& e){
    int dir = (rand() %2 == 0) ? -1 : 1;

    if(isWall(e.x-1, e.y) && isEmpty(e.x, e.y + dir)){
        moveEnemy(e, 0, dir);
    }
}

void updateJumper(enemy& e){
    int distance = abs(p.y-e.y);

    if(distance < 5 && isEmpty(e.x-1, e.y)){
        moveEnemy(e, -1, 0);
    }
}

void drawBoss(const enemy& e){
    for(int i = -1; i <=1; i++){
        for(int j = -1; j <=1; j++){
            drawAt(e.x + i, e.y + j, 'B');
        }
    }
}

void updateBoss(enemy& e, double dt){
    e.timer += dt;
    if(e.timer > 1.0){
        e.timer = 0;
        int dir = (p.y < e.y) ? -1 : 1;
        e.y += dir;
        drawBoss(e);
    }
}

void updateEnemies(double dt) {
    for (auto& e : enemies) {

        if(e.timer < ENEMY_MOVE_DELAY){
            continue;
        }

        switch (e.type) {
        case WALKER:  updateWalker(e); break;
        case FLYER:   updateFlyer(e, dt); break;
        case CRAWLER: updateCrawler(e); break;
        case JUMPER:  updateJumper(e); break;
        case BOSS:    updateBoss(e, dt); break;
        }

        e.timer = 0;
    }
}

void damageEnemies(int ax, int ay){
    for(int i = 0; i < enemies.size(); i++){
        if(enemies[i].x == ax && enemies[i].y == ay){
            enemies[i].hp--;
            if(enemies[i].hp <= 0){
                drawAt(enemies[i].x, enemies[i].y, ' ');
                enemies.erase(enemies.begin() + i);
                return;
            }
        }
    }
}
// ================= MAIN =================

void startWave(int wave){
    enemies.clear();

    int baseCount = 5;
    int extra = randRange(2, 3) * (wave-1);
    int enemyCount = baseCount + extra;

    EnemyType maxType = (EnemyType)(wave-1);
    if(maxType > JUMPER) maxType = JUMPER;

    for(int i = 0; i < enemyCount; i++){
        spawnRandomEnemy(maxType);
    }

    if(wave == MAX_WAVES){
        // spawn boss at center
        spawnEnemy(2, MAPLENGHT /2, BOSS);
    }

    waveActive = true;
}

void updateWave(){
    if(!waveActive && currentWave <= MAX_WAVES){
        startWave(currentWave);
    }

    if(waveActive && enemies.empty()){
        waveActive = false;
        currentWave++;
    }

    if(currentWave > MAX_WAVES){
        // Game won
        cout << "\nYou have defeated all waves! You win!\n";
        exit(0);
    }
}

int main() {
    // I want 2 number 9s, a number 9 large,
    // a number 6 with extra dip,
    // a number 7, two number 45s, one with cheese,
    // and a large soda.

    srand((unsigned)time(0));
    system("cls");

    generateUi();
    printMap();

    lastFrameTime = clock();
    double gravityTimer = 0;

    while (true) {
        clock_t now = clock();
        double deltaTime = double(now - lastFrameTime) / CLOCKS_PER_SEC;
        if (deltaTime < FRAME_TIME) continue;
        lastFrameTime = now;

        updateWave();

        if (_kbhit()) {
            char key = _getch();
            movement(key);
            attack(key);
        }

        gravityTimer += deltaTime;
        if (gravityTimer >= 0.3) {
            if (map[p.x + 1][p.y] == ' ')
                movement('s');
            else
                jumpCount = 0;
            gravityTimer = 0;
        }

        updateAttack(deltaTime);
        updateEnemies(deltaTime);
    }
}
