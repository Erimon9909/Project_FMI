/**
*
* Solution to course project # 10
* Introduction to programming course
* Faculty of Mathematics and Informatics of Sofia University
* Winter semester 2025/2026
*
* @author Borislav Sugarev
* @idnumber 6MI0600690
* @compiler gpp
*
* Implements an ASCII combat game.
*
*/
#include <iostream>
#include <ctime>
#include <conio.h>
#include <cstdlib>
#include <windows.h>

using std::cout;
using std::endl;

const int MAX_JUMPS = 2;
const int UI_HEIGHT = 1;
const int MAPHEIGHT = 20;
const int MAPLENGHT = 95;
const double ENEMY_MOVE_DELAY = 0.25;
const int MAX_WAVES = 5;
const int TARGET_FPS = 60;
const double FRAME_TIME = 1.0 / TARGET_FPS;
const double INVINCIBLE_DURATION = 1.5;

char map[MAPHEIGHT][MAPLENGHT];
clock_t lastFrameTime;

int jumpCount = 0;
int currentWave = 1;
bool waveActive = false;
int playerHP = 5;
double invincibleTimer = 0;

enum EnemyType { WALKER, FLYER, CRAWLER, JUMPER, BOSS };

struct enemy {
    int x, y, hp;
    int dir = 1;
    EnemyType type;
    char icon;
    double timer;
};

struct player {
    int x = MAPHEIGHT - 2;
    int y = MAPLENGHT / 2;
    char icon = '@';
};

player p;

enemy* enemies = nullptr;
int enemiesSize = 0;
int enemiesCapacity = 10;

void reSize() {
    int newCapacity = enemiesCapacity * 2;
    enemy* newArr = new enemy[newCapacity];
    for (int i = 0; i < enemiesSize; i++) {
        newArr[i] = enemies[i];
    }
    delete[] enemies;
    enemies = newArr;
    enemiesCapacity = newCapacity;
}

void addEnemy(enemy e) {
    if (enemiesSize >= enemiesCapacity) {
        reSize();
    }
    enemies[enemiesSize++] = e;
}

void removeEnemy(int index) {
    if (index < 0 || index >= enemiesSize) return;
    for (int i = index; i < enemiesSize - 1; i++) {
        enemies[i] = enemies[i + 1];
    }
    enemiesSize--;
}

void damageEnemies(int ax, int ay);

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void drawAt(int x, int y, char c) {
    if (x < 0 || x >= MAPHEIGHT || y < 0 || y >= MAPLENGHT) return;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)y, (short)(x + UI_HEIGHT) });
    cout << c;
}

void drawEnemy(enemy& e) {
    if (e.type == WALKER) setColor(8);
    else if (e.type == FLYER) setColor(9);
    else if (e.type == CRAWLER) setColor(11);
    else if (e.type == JUMPER) setColor(10);
    else if (e.type == BOSS) setColor(14);

    if (e.type == BOSS) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) drawAt(e.x + i, e.y + j, 'B');
        }
    }
    else drawAt(e.x, e.y, e.icon);
    setColor(7);
}

int randRange(int min, int max) {
    return rand() % (max - min + 1) + min;
}

bool isSolid(int x, int y) {
    if (x < 0 || x >= MAPHEIGHT || y < 0 || y >= MAPLENGHT) return true;
    return map[x][y] == '#' || map[x][y] == '=';
}

bool isEmpty(int x, int y) {
    if (x < 0 || x >= MAPHEIGHT || y < 0 || y >= MAPLENGHT) return false;
    return map[x][y] == ' ';
}

void generateUi() {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
    setColor(7);
    cout << "Wave: " << currentWave << " | Player HP: ";
    for (int i = 0; i < 5; i++) {
        if (i < playerHP) { setColor(12); cout << "O"; }
        else { setColor(8); cout << "o"; }
        setColor(7);
        if (i != 4) cout << "-";
    }
    cout << "   (WASD Move, IJKL Attack)          \n";
}

void playerTakeDamage() {
    if (invincibleTimer > 0) return;
    if (playerHP > 0) {
        playerHP--;
        invincibleTimer = INVINCIBLE_DURATION;
        generateUi();
        if (playerHP <= 0) {
            system("cls");
            cout << "\n\n   GG! YOU DIED.   \n\n";
            exit(0);
        }
    }
}

void generatePlatforms() {
    struct Platform { int x, y, length; };
    Platform platforms[] = { { 7, 20, 14 }, { 7, 55, 14 }, { 5, 35, 16 }, { 10, 10, 12 }, { 13, 15, 14 }, { 16, 8, 15 } };
    for (auto& pl : platforms) {
        for (int i = 0; i < pl.length; i++) map[pl.x][pl.y + i] = '=';
    }
}

void generateMap() {
    for (int i = 0; i < MAPHEIGHT; i++) {
        for (int j = 0; j < MAPLENGHT; j++) {
            if (i == 0 || i == MAPHEIGHT - 1 || j == 0 || j == MAPLENGHT - 1) map[i][j] = '#';
            else map[i][j] = ' ';
        }
    }
    generatePlatforms();
    map[p.x][p.y] = p.icon;
}

void printMap() {
    system("cls");
    generateUi();
    for (int i = 0; i < MAPHEIGHT; i++) {
        for (int j = 0; j < MAPLENGHT; j++) {
            if (map[i][j] == '#' || map[i][j] == '=') setColor(7);
            if (map[i][j] == '@') setColor(15);
            cout << map[i][j];
        }
        cout << endl;
    }
}

bool attacking = false;
double attackTimer = 0;

void clearAttack() {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (p.x + i != p.x || p.y + j != p.y) {
                drawAt(p.x + i, p.y + j, map[p.x + i][p.y + j]);
            }
        }
    }
}

void moveEnemy(enemy& e, int nx, int ny) {
    if (!isEmpty(nx, ny)) return;
    if (e.type == BOSS) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) drawAt(e.x + i, e.y + j, ' ');
        }
    }
    else drawAt(e.x, e.y, ' ');
    e.x = nx; e.y = ny;
    drawEnemy(e);
}

void updateWalker(enemy& e) {
    if (isEmpty(e.x + 1, e.y)) { moveEnemy(e, e.x + 1, e.y); return; }
    if (isSolid(e.x, e.y + e.dir) || isEmpty(e.x + 1, e.y + e.dir)) e.dir *= -1;
    moveEnemy(e, e.x, e.y + e.dir);
}

void updateFlyer(enemy& e) {
    int nx = e.x, ny = e.y;
    if (p.y < e.y) ny--; else if (p.y > e.y) ny++;
    if (abs(p.y - e.y) < 6) { if (e.x < p.x) nx++; else if (e.x > p.x) nx--; }
    else if (e.x > 4) nx--;
    moveEnemy(e, nx, ny);
}

void updateCrawler(enemy& e) {
    int dx[] = { 0, 0, 1, -1 }, dy[] = { 1, -1, 0, 0 };
    int r = rand() % 4;
    int nx = e.x + dx[r], ny = e.y + dy[r];
    if (isEmpty(nx, ny) && (isSolid(nx + 1, ny) || isSolid(nx - 1, ny) || isSolid(nx, ny + 1) || isSolid(nx, ny - 1))) moveEnemy(e, nx, ny);
}

void updateJumper(enemy& e) {
    if (isSolid(e.x + 1, e.y)) {
        if (abs(p.y - e.y) < 6) moveEnemy(e, e.x - 3, e.y + ((p.y > e.y) ? 2 : -2));
    }
    else moveEnemy(e, e.x + 1, e.y);
}

void updateBoss(enemy& e) {
    bool bossGrounded = false;
    for (int j = -1; j <= 1; j++) if (isSolid(e.x + 2, e.y + j)) bossGrounded = true;
    int nx = e.x, ny = e.y;
    if (!bossGrounded) nx++;
    else {
        if (p.y < e.y && isEmpty(e.x, e.y - 2)) ny--;
        else if (p.y > e.y && isEmpty(e.x, e.y + 2)) ny++;
        if (rand() % 10 == 0 && isEmpty(e.x - 1, e.y)) nx--;
    }
    moveEnemy(e, nx, ny);
}

void updateEnemies(double dt) {
    for (int i = 0; i < enemiesSize; i++) {
        enemy& e = enemies[i];
        e.timer += dt;
        if (e.timer < ENEMY_MOVE_DELAY) continue;

        int nextEx = e.x, nextEy = e.y;
        if (e.type == WALKER) nextEy += e.dir;

        if (e.type == WALKER) updateWalker(e);
        else if (e.type == FLYER) updateFlyer(e);
        else if (e.type == CRAWLER) updateCrawler(e);
        else if (e.type == JUMPER) updateJumper(e);
        else if (e.type == BOSS) updateBoss(e);

        bool hit = false;
        if (e.type == BOSS) {
            if (p.x >= e.x - 1 && p.x <= e.x + 1 && p.y >= e.y - 1 && p.y <= e.y + 1) hit = true;
        }
        else if ((e.x == p.x && e.y == p.y) || (nextEx == p.x && nextEy == p.y)) hit = true;

        if (hit) playerTakeDamage();
        e.timer = 0;
    }
}

void damageEnemies(int ax, int ay) {
    for (int i = 0; i < enemiesSize; i++) {
        bool hit = false;
        if (enemies[i].type == BOSS) {
            if (ax >= enemies[i].x - 1 && ax <= enemies[i].x + 1 && ay >= enemies[i].y - 1 && ay <= enemies[i].y + 1) hit = true;
        }
        else if (enemies[i].x == ax && enemies[i].y == ay) hit = true;

        if (hit) {
            enemies[i].hp--;
            if (enemies[i].hp <= 0) {
                if (enemies[i].type == BOSS) {
                    for (int r = -1; r <= 1; r++)
                        for (int c = -1; c <= 1; c++) drawAt(enemies[i].x + r, enemies[i].y + c, ' ');
                }
                else drawAt(enemies[i].x, enemies[i].y, ' ');
                removeEnemy(i);
                i--;
            }
        }
    }
}

void drawAttack(char d) {
    int px = p.x, py = p.y;
    setColor(12);
    if (d == 'i') { damageEnemies(px - 1, py - 1); damageEnemies(px - 1, py); damageEnemies(px - 1, py + 1); drawAt(px - 1, py - 1, '/'); drawAt(px - 1, py, '-'); drawAt(px - 1, py + 1, '\\'); }
    if (d == 'k') { damageEnemies(px + 1, py - 1); damageEnemies(px + 1, py); damageEnemies(px + 1, py + 1); drawAt(px + 1, py - 1, '\\'); drawAt(px + 1, py, '_'); drawAt(px + 1, py + 1, '/'); }
    if (d == 'j') { damageEnemies(px - 1, py - 1); damageEnemies(px, py - 1); damageEnemies(px + 1, py - 1); drawAt(px - 1, py - 1, '/'); drawAt(px, py - 1, '|'); drawAt(px + 1, py - 1, '\\'); }
    if (d == 'l') { damageEnemies(px - 1, py + 1); damageEnemies(px, py + 1); damageEnemies(px + 1, py + 1); drawAt(px - 1, py + 1, '\\'); drawAt(px, py + 1, '|'); drawAt(px + 1, py + 1, '/'); }
    setColor(7);
}

void attack(char input) {
    if (!attacking && (input == 'i' || input == 'j' || input == 'k' || input == 'l')) {
        attacking = true; attackTimer = 0; drawAttack(input);
    }
}

void spawnEnemy(int x, int y, EnemyType type) {
    enemy e;
    e.x = x; e.y = y; e.type = type;
    e.hp = (type == BOSS) ? 15 : 1;
    e.timer = 0;
    e.icon = (type == WALKER) ? 'E' : (type == FLYER) ? 'V' : (type == CRAWLER) ? 'C' : (type == JUMPER) ? 'J' : 'B';
    addEnemy(e);
    drawEnemy(e);
}

void spawnRandomEnemy(EnemyType maxType) {
    int x, y;
    EnemyType t = (EnemyType)randRange(0, (int)maxType);
    bool valid = false;
    while (!valid) {
        x = randRange(2, MAPHEIGHT - 3); y = randRange(2, MAPLENGHT - 3);
        if (isEmpty(x, y)) {
            if (t == CRAWLER) { if (isSolid(x, y + 1) || isSolid(x, y - 1) || isSolid(x + 1, y) || isSolid(x - 1, y)) valid = true; }
            else if (t == WALKER || t == JUMPER) { if (isSolid(x + 1, y)) valid = true; }
            else valid = true;
        }
    }
    spawnEnemy(x, y, t);
}

void startWave(int wave) {
    enemiesSize = 0;
    waveActive = true;
    int count = 3 + wave;
    EnemyType maxT = (wave >= 4) ? JUMPER : (EnemyType)(wave - 1);
    for (int i = 0; i < count; i++) spawnRandomEnemy(maxT);
    if (wave == MAX_WAVES) spawnEnemy(3, MAPLENGHT / 2, BOSS);
}

void updateWave() {
    if (!waveActive && currentWave <= MAX_WAVES) startWave(currentWave);
    if (waveActive && enemiesSize == 0) { waveActive = false; currentWave++; printMap(); }
    if (currentWave > MAX_WAVES) { system("cls"); cout << "VICTORY!"; exit(0); }
}

int main() {
    enemies = new enemy[enemiesCapacity];
    srand((unsigned)time(0));
    generateMap();
    printMap();
    lastFrameTime = clock();
    double gTimer = 0;

    while (true) {
        clock_t now = clock();
        double dt = double(now - lastFrameTime) / CLOCKS_PER_SEC;
        if (dt < FRAME_TIME) continue;
        lastFrameTime = now;
        updateWave();

        int moveX = 0, moveY = 0;
        if (GetAsyncKeyState('A') & 0x8000) moveY = -1;
        if (GetAsyncKeyState('D') & 0x8000) moveY = 1;

        static bool wWasDown = false;
        bool wIsDown = (GetAsyncKeyState('W') & 0x8000);
        if (wIsDown && !wWasDown && jumpCount < MAX_JUMPS) {
            int steps = 0;
            for (int i = 1; i <= 3; i++) { if (isEmpty(p.x - i, p.y)) steps = i; else break; }
            if (steps > 0) { moveX = -steps; jumpCount++; }
        }
        wWasDown = wIsDown;

        gTimer += dt;
        if (gTimer >= 0.15) {
            if (isEmpty(p.x + 1, p.y)) moveX = 1;
            else jumpCount = 0;
            gTimer = 0;
        }

        if (moveX != 0 || moveY != 0) {
            int nx = p.x + moveX, ny = p.y + moveY;
            if (!isEmpty(nx, ny)) {
                if (isEmpty(nx, p.y)) ny = p.y;
                else if (isEmpty(p.x, ny)) nx = p.x;
                else { nx = p.x; ny = p.y; }
            }
            if (nx != p.x || ny != p.y) {
                if (attacking) { clearAttack(); attacking = false; attackTimer = 0; }
                drawAt(p.x, p.y, ' '); map[p.x][p.y] = ' ';
                p.x = nx; p.y = ny;
                map[p.x][p.y] = '@'; setColor(15); drawAt(p.x, p.y, '@'); setColor(7);
            }
        }

        if (_kbhit()) attack(_getch());

        if (invincibleTimer > 0) {
            invincibleTimer -= dt;
            if ((int)(invincibleTimer * 10) % 2 == 0) { setColor(15); drawAt(p.x, p.y, p.icon); }
            else drawAt(p.x, p.y, ' ');
            if (invincibleTimer <= 0) { invincibleTimer = 0; setColor(15); drawAt(p.x, p.y, p.icon); }
        }

        if (attacking) {
            attackTimer += dt;
            if (attackTimer >= 0.12) { clearAttack(); attacking = false; attackTimer = 0; }
        }

        updateEnemies(dt);
    }
    delete[] enemies;
    return 0;
}
