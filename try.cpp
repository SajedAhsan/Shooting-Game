#include "iGraphics.h"
#include "iSound.h"
#include <stdlib.h>
#include <time.h>

//flag for combine
bool isGameRunning = false;
bool isGameOver = false;
int gameOverTimer = 0;


//menu variables
#define BUTTON_COUNT 5
const char *menuItems[BUTTON_COUNT] = {"PLAY", "HIGH SCORE", "HELP", "CREDITS" ,"EXIT"};
int menuYStart = 350;
int menuX = 550;
int menuWidth = 150;
int menuHeight = 40;
int menuSpacing = 20;
int hoveredIndex = -1;
Image menu_background;
Image gameover;

//game variables
Image bg;
Image soldier_idle[1], soldier_run[8], soldier_jump[5], fire[3], bullet[1],soldier_dead[4];

Sprite soldier_i, soldier_r, soldier_j, soldier_fr, bul,soldier_d;

Image zombie_run[7], zombie_dead[5],zombie_attack[5];

#define MAX_ZOMBIES 10
Sprite zombie_r[MAX_ZOMBIES], zombie_d[MAX_ZOMBIES],zombie_a[MAX_ZOMBIES];
bool zombie_dead_state[MAX_ZOMBIES];
bool zombie_dead_animation_done[MAX_ZOMBIES];
int zombie_dead_frame_counter[MAX_ZOMBIES];
int zombie_position_x[MAX_ZOMBIES];
int zombie_position_y[MAX_ZOMBIES];
bool zombie_attacking[MAX_ZOMBIES];
int zombie_attack_counter[MAX_ZOMBIES];
int attack_frame_delay[MAX_ZOMBIES];
int attack_frame_timer[MAX_ZOMBIES];
int total_zombies = 0;

#define soldier 1
bool isMirroredX[2] = {false, false};
int bg_flag = 0;
bool is_running = false;
bool is_firing = false;
bool jump = false;
bool zombie_should_move = false;
#define MAX_BULLETS 40
bool bullet_fired[MAX_BULLETS];
int bullet_positionx[MAX_BULLETS];
int bullet_positiony[MAX_BULLETS];
int soldier_position_x = 0;
int soldier_position_y = 128;

bool soldier_is_dying=false;
bool soldier_is_dead = false;
int soldier_death_frame = 0;

bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return !(x1 + w1 < x2 || x1 > x2 + w2 || y1 + h1 < y2 || y1 > y2 + h2);
}

void zombieAttackUpdate() {
    if (soldier_is_dead) return;

    for (int i = 0; i < total_zombies; i++) {
        if (!zombie_dead_state[i]) {
            if (checkCollision(zombie_position_x[i], zombie_position_y[i], 100, 100,
                               soldier_position_x, soldier_position_y, 100, 100)) {
                zombie_attacking[i] = true;
                soldier_is_dead = true;
                attack_frame_timer[i]++;
                if (attack_frame_timer[i] >= attack_frame_delay[i]) {
                    attack_frame_timer[i] = 0;
                    zombie_attack_counter[i]++;
                }
            } else {
                zombie_attacking[i] = false;
                zombie_attack_counter[i] = 0;
                attack_frame_timer[i] = 0;
            }
        }
    }
}

void spawnZombies() {
    total_zombies = 3 + rand() % (MAX_ZOMBIES - 3);
    for (int i = 0; i < total_zombies; i++) {
        zombie_position_x[i] = 1400 + rand() % 400 + i * 150;
        zombie_position_y[i] = 128;
        zombie_dead_state[i] = false;
        zombie_dead_animation_done[i] = false;
        zombie_dead_frame_counter[i] = 0;
        zombie_attacking[i] = false;
        zombie_attack_counter[i] = 0;
        attack_frame_timer[i] = 0;
        attack_frame_delay[i] = 30;

        iInitSprite(&zombie_r[i], -1);
        iChangeSpriteFrames(&zombie_r[i], zombie_run, 7);
        iSetSpritePosition(&zombie_r[i], zombie_position_x[i], zombie_position_y[i]);
        iScaleSprite(&zombie_r[i], 3.75);

        iInitSprite(&zombie_d[i], -1);
        iChangeSpriteFrames(&zombie_d[i], zombie_dead, 5);
        iSetSpritePosition(&zombie_d[i], zombie_position_x[i], zombie_position_y[i]);
        iScaleSprite(&zombie_d[i], 3.75);

        iInitSprite(&zombie_a[i], -1);
        iChangeSpriteFrames(&zombie_a[i], zombie_attack, 5);
        iSetSpritePosition(&zombie_a[i], zombie_position_x[i], zombie_position_y[i]);
        iScaleSprite(&zombie_a[i], 3.75);
    }
}
void loadResources() {
    srand(time(0));
    iLoadImage(&bg, "back.png");
    iResizeImage(&bg, 1400, 600);

    iLoadFramesFromFolder(soldier_run, "this_img/run");
    iInitSprite(&soldier_r, -1);
    iChangeSpriteFrames(&soldier_r, soldier_run, 8);
    iSetSpritePosition(&soldier_r, soldier_position_x, soldier_position_y);
    iScaleSprite(&soldier_r, 3.5);

    iLoadFramesFromFolder(soldier_idle, "this_img/atack");
    iInitSprite(&soldier_i, -1);
    iChangeSpriteFrames(&soldier_i, soldier_idle, 1);
    iSetSpritePosition(&soldier_i, soldier_position_x, soldier_position_y);
    iScaleSprite(&soldier_i, 3.5);

    iLoadFramesFromFolder(fire, "this_img/attack");
    iInitSprite(&soldier_fr, -1);
    iChangeSpriteFrames(&soldier_fr, fire, 3);
    iSetSpritePosition(&soldier_fr, soldier_position_x, soldier_position_y);
    iScaleSprite(&soldier_fr, 3.5);

    iLoadFramesFromFolder(soldier_dead, "this_img/dead");
    iInitSprite(&soldier_d, -1);
    iChangeSpriteFrames(&soldier_d, soldier_dead, 4);
    iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
    iScaleSprite(&soldier_d,3.5);

    iLoadFramesFromFolder(bullet, "this_img/bullet");
    iInitSprite(&bul, -1);
    iChangeSpriteFrames(&bul, bullet, 1);
    iScaleSprite(&bul, 1);
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullet_fired[i] = false;
        bullet_positionx[i] = -100;
        bullet_positiony[i] = 250;
    }

    iLoadFramesFromFolder(zombie_run, "this_img/z_run");
    iLoadFramesFromFolder(zombie_dead, "this_img/z_dead");
    iLoadFramesFromFolder(zombie_attack,"this_img/z_attack");
    spawnZombies();
    iLoadImage(&menu_background, "background.png");
    //iLoadImage(&menu_background, "assets/images/menu.png");
    iResizeImage(&menu_background, 1100, 550);

    iLoadImage(&gameover, "gameover.png");
    //iLoadImage(&gameover, "gameover1.png");
    iResizeImage(&gameover, 1100,550);

     isGameOver = false;
    gameOverTimer = 0;


}

// rest of the code remains unchanged


void iDraw() {
    iClear();

    if (isGameOver) {
        iShowLoadedImage(50, 50, &gameover);
        return;
    }

    if (!isGameRunning) {
        iShowLoadedImage(80, 20, &menu_background);
        for (int i = 0; i < BUTTON_COUNT; i++) {
            int y = menuYStart - i * (menuHeight + menuSpacing);
            if (i == hoveredIndex) iSetColor(20, 250, 235);
            else iSetColor(255, 255, 255);
            iText(menuX, y, menuItems[i], GLUT_BITMAP_HELVETICA_18);
        }
    } else {
        iShowLoadedImage(0, 0, &bg);

        if (soldier_is_dead) {
            iShowSprite(&soldier_d);
        } else if (is_firing) {
            iShowSprite(&soldier_fr);
        } else if (is_running) {
            iShowSprite(&soldier_r);
            iWrapImage(&bg, -3);
        } else {
            iShowSprite(&soldier_i);
        }

        for (int i = 0; i < total_zombies; i++) {
            if (!zombie_dead_state[i]) {
                if (zombie_attacking[i]) {
                    iShowSprite(&zombie_a[i]);
                } else {
                    iShowSprite(&zombie_r[i]);
                }
            } else if (!zombie_dead_animation_done[i]) {
                iShowSprite(&zombie_d[i]);
            }
        }

        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullet_fired[i]) {
                iSetSpritePosition(&bul, bullet_positionx[i], bullet_positiony[i]);
                iShowSprite(&bul);
            }
        }
    }
}

void iMouseMove(int mx, int my) 
{
    if (!isGameRunning) 
    {
        hoveredIndex = -1;
        for (int i = 0; i < BUTTON_COUNT; i++) {
            int y = menuYStart - i * (menuHeight + menuSpacing);
            if (mx >= menuX && mx <= menuX + menuWidth && my >= y - 5 && my <= y + 20) {
                hoveredIndex = i;
                break;
            }
        }
    }
}

void iMouse(int button, int state, int mx, int my) 
{
    if (!isGameRunning && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && hoveredIndex != -1) {
        iPlaySound("assets/sounds/clicked.wav", false);
        if (hoveredIndex == 0)
        {
         isGameRunning = true;
        }
        else if (hoveredIndex == 1)
        {
            // High Score
        }
        else if(hoveredIndex == 2)
        {
            // Help
        }
        else if(hoveredIndex == 3)
        {
            // credit
        }
        else if (hoveredIndex == 4)
         exit(0);
    }
}
void iMouseDrag(int mx, int my){};
void iMouseWheel(int dir, int mx, int my){};

void iKeyboard(unsigned char key) {
    if (!isGameRunning) return;
    if (key == 's') 
    { is_running = false;
         is_firing = false; }

    if (key == 'd')
     { is_running = true; 
        is_firing = false;
        bg_flag = 1;
     zombie_should_move = true;

        if (isMirroredX[soldier]) 
        { iMirrorSprite(&soldier_r, HORIZONTAL);
             isMirroredX[soldier] = false; }
    }
    if (key == 'a') 
    { is_firing = false;
         is_running = true;
          bg_flag = 2;
        if (!isMirroredX[soldier]) 
        { iMirrorSprite(&soldier_r, HORIZONTAL); 
            isMirroredX[soldier] = true; 
        }
    }
    if (key == 'f') 
    {
        for (int i = 0; i < MAX_BULLETS; i++) 
        {
            if (!bullet_fired[i]) {
                bullet_fired[i] = true;
                bullet_positionx[i] = soldier_position_x + 150;
                bullet_positiony[i] = 250;
                break;
            }
        }
        is_firing = true; is_running = false;
        zombie_should_move = true;
    }
}

void iSpecialKeyboard(unsigned char key) {
    if (!isGameRunning) {return;}

    if (key == GLUT_KEY_END){
     exit(0);
    }
    
     if (key == GLUT_KEY_LEFT)
     {
        bg_flag = 2;
        if (!isMirroredX[soldier]) { iMirrorSprite(&soldier_r, HORIZONTAL); isMirroredX[soldier] = true; }
        if (soldier_position_x != 0) {
            soldier_position_x -= 7;
            soldier_r.x -= 7; soldier_i.x -= 7; soldier_fr.x -= 7;
        }
    }

    if (key == GLUT_KEY_RIGHT) {
        is_running = true; bg_flag = 1;
        if (isMirroredX[soldier]) { iMirrorSprite(&soldier_r, HORIZONTAL); isMirroredX[soldier] = false; }
        if (soldier_position_x <= 1200) {
            soldier_position_x += 7;
            soldier_r.x += 7; soldier_i.x += 7; soldier_fr.x += 7;
            zombie_should_move = true;
        }
    }
}


void iAnim() {
    if (!isGameRunning) return;

    if (!soldier_is_dead) {
        iAnimateSprite(&soldier_i);
        if (is_running) iAnimateSprite(&soldier_r);
        if (is_firing) iAnimateSprite(&soldier_fr);
    } else {
        iAnimateSprite(&soldier_d);
        soldier_death_frame++;

        if (soldier_death_frame >= 4) {
            gameOverTimer++;
            if (gameOverTimer >= 10) { // 20 * 150ms = 3000ms = 3 sec
                isGameOver = true;
                isGameRunning = false;
            }
        }
    }

    if (!isGameOver) {
        for (int i = 0; i < total_zombies; i++) {
            if (!zombie_dead_state[i]) {
                if (checkCollision(zombie_position_x[i], zombie_position_y[i], 100, 100,
                                   soldier_position_x, soldier_position_y, 100, 100)) {
                    zombie_attacking[i] = true;
                    attack_frame_timer[i]++;
                    if (attack_frame_timer[i] >= attack_frame_delay[i]) {
                        attack_frame_timer[i] = 0;
                        zombie_attack_counter[i]++;
                        if (zombie_attack_counter[i] >= 3) {
                            soldier_is_dead = true;
                        }
                    }
                    iAnimateSprite(&zombie_a[i]);
                } else {
                    zombie_attacking[i] = false;
                    zombie_attack_counter[i] = 0;
                    attack_frame_timer[i] = 0;
                    iAnimateSprite(&zombie_r[i]);
                }
            } else if (!zombie_dead_animation_done[i]) {
                iAnimateSprite(&zombie_d[i]);
                zombie_dead_frame_counter[i]++;
                if (zombie_dead_frame_counter[i] >= 5) {
                    zombie_dead_animation_done[i] = true;
                    zombie_dead_state[i] = false;
                    zombie_dead_animation_done[i] = false;
                    zombie_dead_frame_counter[i] = 0;
                    zombie_position_x[i] = 1400 + rand() % 300;
                    iSetSpritePosition(&zombie_r[i], zombie_position_x[i], zombie_position_y[i]);
                    iSetSpritePosition(&zombie_d[i], zombie_position_x[i], zombie_position_y[i]);
                    iSetSpritePosition(&zombie_a[i], zombie_position_x[i], zombie_position_y[i]);
                }
            }
        }
    }
}


void bullet_change_position() {
    if (!isGameRunning) return;

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullet_fired[i]) {
            bullet_positionx[i] += 25;

            if (bullet_positionx[i] > 1400) {
                bullet_fired[i] = false;
                bullet_positionx[i] = -100;
            }

            for (int j = 0; j < total_zombies; j++) {
                if (!zombie_dead_state[j] && bullet_positionx[i] >= zombie_position_x[j] && bullet_positionx[i] <= zombie_position_x[j] + 100) {
                    bullet_fired[i] = false;
                    bullet_positionx[i] = -100;
                    zombie_dead_state[j] = true;
                    zombie_dead_frame_counter[j] = 0;
                    iSetSpritePosition(&zombie_d[j], zombie_position_x[j], zombie_position_y[j]);
                }
            }
        }
    }

    if (zombie_should_move) {
        for (int i = 0; i < total_zombies; i++) {
            if (!zombie_dead_state[i] && !zombie_attacking[i]) {
                zombie_position_x[i] -= 40;
                zombie_r[i].x -= 40;
                zombie_d[i].x -= 40;
                zombie_a[i].x -= 40;
            }
        }
    }
}

int main(int argc, char *argv[]) 
{
    glutInit(&argc, argv);
    loadResources();
    iInitializeSound();
    iSetTimer(150, iAnim);
    iSetTimer(50, bullet_change_position);
    iSetTimer(100, zombieAttackUpdate);

    iInitialize(1400, 600, "Combined Game");
    return 0;
}