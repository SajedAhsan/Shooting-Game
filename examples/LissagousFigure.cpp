#include "iGraphics.h"
#include "iSound.h"
#include <stdlib.h>
#include <time.h>

// flag for combine
bool isGameRunning = false;
bool isGameOver = false;
int gameOverTimer = 0;

// menu variables
#define BUTTON_COUNT 5
const char *menuItems[BUTTON_COUNT] = {"PLAY", "HIGH SCORE", "HELP", "CREDITS", "EXIT"};
int menuYStart = 350;
int menuX = 550;
int menuWidth = 150;
int menuHeight = 40;
int menuSpacing = 20;
int hoveredIndex = -1;
Image menu_background;
Image gameover;

// game variables
Image bg;
Image soldier_idle[1], soldier_run[8], soldier_jump[5], fire[3], bullet[1], soldier_dead[4];

Sprite soldier_i, soldier_r, soldier_j, soldier_fr, bul, soldier_d;

Image zombie_run[7], zombie_dead[5], zombie_attack[5];

#define MAX_ZOMBIES 10
Sprite zombie_r[MAX_ZOMBIES], zombie_d[MAX_ZOMBIES], zombie_a[MAX_ZOMBIES];
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
bool bullet_direction[MAX_BULLETS]; // false: right, true: left
int soldier_position_x = 0;
int soldier_position_y = 128;
bool soldier_is_dying = false;
bool soldier_is_dead = false;
int soldier_death_frame = 0;
int soldier_life = 300; // New: soldier has 3 lives

// Boss sprite variables
Image boss_idle[7], boss_run[8], boss_attack[8], boss_dead[6], boss_cattack[14];
Sprite boss_i, boss_r, boss_a, boss_d, boss_ca;

int boss_x, boss_y = 128;
// int boss_target_x = 900, boss_target_y = 128;
int boss_state = 0; // 0: idle, 1: running, 2: attacking, 3: dead 4: close attack
int boss_frame_timer = 0;
int boss_respawn_timer = 0;
bool boss_alive = false;

#define BOSS_BULLET_MAX 10

int boss_health = 2000;
int boss_health_max = 2000;
int boss_fight_timer = 0; // Add this here for Hermit-style respawn

bool bossMirrored = false;

int boss_dead_frame = 0; // Counter for boss dead animation

int soldier_health = 10;
int soldier_health_max = 10;

// Boss fireball variables
Image boss_fire[4];
Sprite boss_fire_sprite;
bool boss_fire_active = false;
int boss_fire_x = -1000, boss_fire_y = -1000;
bool boss_fire_left = false;

// Add a delay counter for boss dead animation
int boss_dead_delay = 0;

bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return !(x1 + w1 < x2 || x1 > x2 + w2 || y1 + h1 < y2 || y1 > y2 + h2);
}

void zombieAttackUpdate()
{
    if (soldier_is_dead)
        return;

    for (int i = 0; i < total_zombies; i++)
    {
        if (!zombie_dead_state[i])
        {
            if (checkCollision(zombie_position_x[i], zombie_position_y[i], 100, 100,
                               soldier_position_x, soldier_position_y, 100, 100))
            {
                zombie_attacking[i] = true;
                if (!soldier_is_dead)
                {
                    soldier_life--; // Lose a life on attack
                    if (soldier_life <= 0)
                    {
                        soldier_is_dead = true;
                    }
                }
                attack_frame_timer[i]++;
                if (attack_frame_timer[i] >= attack_frame_delay[i])
                {
                    attack_frame_timer[i] = 0;
                    zombie_attack_counter[i]++;
                }
            }
            else
            {
                zombie_attacking[i] = false;
                zombie_attack_counter[i] = 0;
                attack_frame_timer[i] = 0;
            }
        }
    }
}

// void spawnZombies() {
//     total_zombies = 3 + rand() % (MAX_ZOMBIES - 3);
//     for (int i = 0; i < total_zombies; i++) {
//         zombie_position_x[i] = 1400 + rand() % 400 + i * 150;
//         zombie_position_y[i] = 128;
//         zombie_dead_state[i] = false;
//         zombie_dead_animation_done[i] = false;
//         zombie_dead_frame_counter[i] = 0;
//         zombie_attacking[i] = false;
//         zombie_attack_counter[i] = 0;
//         attack_frame_timer[i] = 0;
//         attack_frame_delay[i] = 30;

//         iInitSprite(&zombie_r[i]);
//         iChangeSpriteFrames(&zombie_r[i], zombie_run, 7);
//         iSetSpritePosition(&zombie_r[i], zombie_position_x[i], zombie_position_y[i]);
//         iScaleSprite(&zombie_r[i], 3.75);

//         iInitSprite(&zombie_d[i]);
//         iChangeSpriteFrames(&zombie_d[i], zombie_dead, 5);
//         iSetSpritePosition(&zombie_d[i], zombie_position_x[i], zombie_position_y[i]);
//         iScaleSprite(&zombie_d[i], 3.75);

//         iInitSprite(&zombie_a[i]);
//         iChangeSpriteFrames(&zombie_a[i], zombie_attack, 5);
//         iSetSpritePosition(&zombie_a[i], zombie_position_x[i], zombie_position_y[i]);
//         iScaleSprite(&zombie_a[i], 3.75);
//     }
// }
void loadResources()
{
    srand(time(0));
    iLoadImage(&bg, "back.png");
    iResizeImage(&bg, 1400, 600);

    iLoadFramesFromFolder(soldier_run, "this_img/run");
    iInitSprite(&soldier_r);
    iChangeSpriteFrames(&soldier_r, soldier_run, 8);
    iSetSpritePosition(&soldier_r, soldier_position_x, soldier_position_y);
    iScaleSprite(&soldier_r, 3.5);

    iLoadFramesFromFolder(soldier_idle, "this_img/atack");
    iInitSprite(&soldier_i);
    iChangeSpriteFrames(&soldier_i, soldier_idle, 1);
    iSetSpritePosition(&soldier_i, soldier_position_x, soldier_position_y);
    iScaleSprite(&soldier_i, 3.5);

    iLoadFramesFromFolder(fire, "this_img/attack");
    iInitSprite(&soldier_fr);
    iChangeSpriteFrames(&soldier_fr, fire, 3);
    iSetSpritePosition(&soldier_fr, soldier_position_x, soldier_position_y);
    iScaleSprite(&soldier_fr, 3.5);

    iLoadFramesFromFolder(soldier_dead, "this_img/dead");
    iInitSprite(&soldier_d);
    iChangeSpriteFrames(&soldier_d, soldier_dead, 4);
    iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
    iScaleSprite(&soldier_d, 3.5);

    iLoadFramesFromFolder(bullet, "this_img/bullet");
    iInitSprite(&bul);
    iChangeSpriteFrames(&bul, bullet, 1);
    iScaleSprite(&bul, 1);
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullet_fired[i] = false;
        bullet_positionx[i] = -100;
        bullet_positiony[i] = 250;
    }

    iLoadFramesFromFolder(zombie_run, "this_img/z_run");
    iLoadFramesFromFolder(zombie_dead, "this_img/z_dead");
    iLoadFramesFromFolder(zombie_attack, "this_img/z_attack");
    // --- BOSS SPRITE LOADING AND INIT ---
    iLoadFramesFromFolder(boss_idle, "this_img/boss_idle");
    iInitSprite(&boss_i);
    iChangeSpriteFrames(&boss_i, boss_idle, 7);
    iSetSpritePosition(&boss_i, boss_x, boss_y);
    iScaleSprite(&boss_i, 4);

    iLoadFramesFromFolder(boss_run,"this_img/boss_run");
    iInitSprite(&boss_r);
    iChangeSpriteFrames(&boss_r, boss_run, 8);
    iSetSpritePosition(&boss_r, boss_x, boss_y);
    iScaleSprite(&boss_r, 4);

    iLoadFramesFromFolder(boss_attack,"this_img/boss_attack");
    iInitSprite(&boss_a);
    iChangeSpriteFrames(&boss_a, boss_attack, 8);
    iSetSpritePosition(&boss_a, boss_x, boss_y);
    iScaleSprite(&boss_a, 4);

    iLoadFramesFromFolder(boss_dead,"this_img/boss_dead");
    iInitSprite(&boss_d);
    iChangeSpriteFrames(&boss_d, boss_dead, 6);
    iSetSpritePosition(&boss_d, boss_x, boss_y);
    iScaleSprite(&boss_d, 4);

    iLoadFramesFromFolder(boss_cattack, "this_img/boss_cattack");
    iInitSprite(&boss_ca);
    iChangeSpriteFrames(&boss_ca, boss_cattack, 14);
    iSetSpritePosition(&boss_ca, boss_x, boss_y);
    iScaleSprite(&boss_ca, 4);

    iLoadFramesFromFolder(boss_fire, "this_img/boss_fire");
    iInitSprite(&boss_fire_sprite);
    iChangeSpriteFrames(&boss_fire_sprite, boss_fire, 4);
    iScaleSprite(&boss_fire_sprite, 3);
    // --- END BOSS SPRITE LOADING ---
    // spawnZombies();
    // iLoadImage(&menu_background, "background.png");
    iLoadImage(&menu_background, "assets/images/menu.png");
    iResizeImage(&menu_background, 1100, 550);

    // iLoadImage(&gameover, "gameover.png");
    iLoadImage(&gameover, "gameover1.png");
    iResizeImage(&gameover, 1100, 550);

    isGameOver = false;
    gameOverTimer = 0;
    boss_dead_frame = 0; // Reset boss dead animation on new game
    boss_dead_delay = 0; // Reset boss dead delay on new game
}

// rest of the code remains unchanged

void iDraw()
{
    iClear();

    if (isGameOver)
    {
        iShowLoadedImage(50, 50, &gameover);
        return;
    }

    if (!isGameRunning)
    {
        iShowLoadedImage(80, 20, &menu_background);
        for (int i = 0; i < BUTTON_COUNT; i++)
        {
            int y = menuYStart - i * (menuHeight + menuSpacing);
            if (i == hoveredIndex)
                iSetColor(20, 250, 235);
            else
                iSetColor(255, 255, 255);
            iText(menuX, y, menuItems[i], GLUT_BITMAP_HELVETICA_18);
        }
    }
    else
    {
        iShowLoadedImage(0, 0, &bg);

        // Draw soldier health bar
        iSetColor(0, 200, 0);
        iFilledRectangle(30, 570, 200 * soldier_health / soldier_health_max, 15);
        iSetColor(255, 255, 255);
        iRectangle(30, 570, 200, 15);
        iSetColor(0, 0, 0);
        iText(35, 573, "Soldier HP", GLUT_BITMAP_HELVETICA_12);
        iSetColor(255, 255, 255);

        // Draw boss health bar (if alive)
        if (boss_alive)
        {
            iSetColor(200, 0, 0);
            iFilledRectangle(900, 570, 200 * boss_health / boss_health_max, 15);
            iSetColor(255, 255, 255);
            iRectangle(900, 570, 200, 15);
            iSetColor(0, 0, 0);
            iText(905, 573, "Boss HP", GLUT_BITMAP_HELVETICA_12);
            iSetColor(255, 255, 255);
        }

        if (soldier_is_dead)
        {
            iShowSprite(&soldier_d);
        }
        else if (is_firing)
        {
            iShowSprite(&soldier_fr);
        }
        else if (is_running)
        {
            iShowSprite(&soldier_r);
            iWrapImage(&bg, -3);
        }
        else
        {
            iShowSprite(&soldier_i);
        }

        for (int i = 0; i < total_zombies; i++)
        {
            if (!zombie_dead_state[i])
            {
                if (zombie_attacking[i])
                {
                    iShowSprite(&zombie_a[i]);
                }
                else
                {
                    iShowSprite(&zombie_r[i]);
                }
            }
            else if (!zombie_dead_animation_done[i])
            {
                iShowSprite(&zombie_d[i]);
            }
        }

        // Draw boss
        if (boss_alive)
        {
            if (boss_state == 0)
                iShowSprite(&boss_i);
            else if (boss_state == 1)
                iShowSprite(&boss_r);
            else if (boss_state == 2)
                iShowSprite(&boss_a);
            else if (boss_state == 4)
                iShowSprite(&boss_ca);
        }
        else
        {
            iShowSprite(&boss_d); // Always show boss dead sprite when boss is dead
        }

        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullet_fired[i])
            {
                iSetSpritePosition(&bul, bullet_positionx[i], bullet_positiony[i]);
                iShowSprite(&bul);
            }
        }

        // Draw boss fireball
        if (boss_fire_active)
        {
            iSetSpritePosition(&boss_fire_sprite, boss_fire_x, boss_fire_y); // <-- Only here!
            iShowSprite(&boss_fire_sprite);
        }
    }
}

void iMouseMove(int mx, int my)
{
    if (!isGameRunning)
    {
        hoveredIndex = -1;
        for (int i = 0; i < BUTTON_COUNT; i++)
        {
            int y = menuYStart - i * (menuHeight + menuSpacing);
            if (mx >= menuX && mx <= menuX + menuWidth && my >= y - 5 && my <= y + 20)
            {
                hoveredIndex = i;
                break;
            }
        }
    }
}

void iMouse(int button, int state, int mx, int my)
{
    if (!isGameRunning && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && hoveredIndex != -1)
    {
        iPlaySound("assets/sounds/clicked.wav", false);
        if (hoveredIndex == 0)
        {
            isGameRunning = true;
        }
        else if (hoveredIndex == 1)
        {
            // High Score
        }
        else if (hoveredIndex == 2)
        {
            // Help
        }
        else if (hoveredIndex == 3)
        {
            // credit
        }
        else if (hoveredIndex == 4)
            exit(0);
    }
}
void iMouseDrag(int mx, int my) {};
void iMouseWheel(int dir, int mx, int my) {};

void iKeyboard(unsigned char key)
{
    if (!isGameRunning)
        return;
    if (key == 's')
    {
        is_running = false;
        is_firing = false;
    }

    if (key == 'd')
    {
        is_running = true;
        is_firing = false;
        bg_flag = 1;
        zombie_should_move = true;
        if (isMirroredX[soldier])
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL);
            isMirroredX[soldier] = false;
        }
    }
    if (key == 'a')
    {
        is_firing = false;
        is_running = true;
        bg_flag = 2;
        if (!isMirroredX[soldier])
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL);
            isMirroredX[soldier] = true;
        }
    }
    if (key == 'f')
    {
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullet_fired[i])
            {
                bullet_fired[i] = true;
                if (isMirroredX[soldier])
                {
                    bullet_positionx[i] = soldier_position_x;
                    bullet_direction[i] = true; // left
                }
                else
                {
                    bullet_positionx[i] = soldier_position_x + 150;
                    bullet_direction[i] = false; // right
                }
                bullet_positiony[i] = 250;
                break;
            }
        }
        is_firing = true;
        is_running = false;
        zombie_should_move = true;
    }
}

void iSpecialKeyboard(unsigned char key)
{
    if (!isGameRunning)
    {
        return;
    }

    if (key == GLUT_KEY_END)
    {
        exit(0);
    }

    if (key == GLUT_KEY_LEFT)
    {
        bg_flag = 2;
        if (!isMirroredX[soldier])
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL);
            isMirroredX[soldier] = true;
        }
        if (soldier_position_x != 0)
        {
            soldier_position_x -= 7;
            soldier_r.x -= 7;
            soldier_i.x -= 7;
            soldier_fr.x -= 7;
        }
    }

    if (key == GLUT_KEY_RIGHT)
    {
        is_running = true;
        bg_flag = 1;
        if (isMirroredX[soldier])
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL);
            isMirroredX[soldier] = false;
        }
        if (soldier_position_x <= 1200)
        {
            soldier_position_x += 7;
            soldier_r.x += 7;
            soldier_i.x += 7;
            soldier_fr.x += 7;
            zombie_should_move = true;
        }
    }
}

void iAnim()
{
    if (!isGameRunning)
        return;

    if (isGameOver)
        return; // Prevent further animation after game over

    if (!soldier_is_dead)
    {
        iAnimateSprite(&soldier_i);
        if (is_running)
            iAnimateSprite(&soldier_r);
        if (is_firing)
            iAnimateSprite(&soldier_fr);
    }
    else
    {
        iAnimateSprite(&soldier_d);
        soldier_death_frame++;

        if (soldier_death_frame >= 4)
        {
            gameOverTimer++;
            if (gameOverTimer >= 10)
            {
                isGameOver = true;
                isGameRunning = false;
                return;
            }
        }
    }

    // Animate boss sprite according to its state
    if (boss_alive)
    {
        if (boss_state == 0)
            iAnimateSprite(&boss_i);
        else if (boss_state == 1)
            iAnimateSprite(&boss_r);
        else if (boss_state == 2)
            iAnimateSprite(&boss_a);
        else if (boss_state == 4)
            iAnimateSprite(&boss_ca);
    }

    if (!isGameOver)
    {
        for (int i = 0; i < total_zombies; i++)
        {
            if (!zombie_dead_state[i])
            {
                if (checkCollision(zombie_position_x[i], zombie_position_y[i], 100, 100,
                                   soldier_position_x, soldier_position_y, 100, 100))
                {
                    zombie_attacking[i] = true;
                    attack_frame_timer[i]++;
                    if (attack_frame_timer[i] >= attack_frame_delay[i])
                    {
                        attack_frame_timer[i] = 0;
                        zombie_attack_counter[i]++;
                        if (zombie_attack_counter[i] >= 3)
                        {
                            soldier_is_dead = true;
                        }
                    }
                    iAnimateSprite(&zombie_a[i]);
                }
                else
                {
                    zombie_attacking[i] = false;
                    zombie_attack_counter[i] = 0;
                    attack_frame_timer[i] = 0;
                    iAnimateSprite(&zombie_r[i]);
                }
            }
            else if (!zombie_dead_animation_done[i])
            {
                iAnimateSprite(&zombie_d[i]);
                zombie_dead_frame_counter[i]++;
                if (zombie_dead_frame_counter[i] >= 5)
                {
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

void bossUpdate()
{
    if (!boss_alive)
    {
        boss_respawn_timer++;
        if (boss_respawn_timer > 5)
        {
            boss_x = rand() % 1300;
            boss_y = 128;
            iSetSpritePosition(&boss_i, boss_x, boss_y);
            iSetSpritePosition(&boss_r, boss_x, boss_y);
            iSetSpritePosition(&boss_a, boss_x, boss_y);
            iSetSpritePosition(&boss_d, boss_x, boss_y);
            iSetSpritePosition(&boss_ca, boss_x, boss_y);
            boss_alive = true;
            boss_state = 1;
            boss_respawn_timer = 0;
            boss_health = boss_health_max;
            bossMirrored = false;
            boss_frame_timer = 0;
            boss_fight_timer = 0;
        }
        return;
    }
    boss_frame_timer++;
    boss_fight_timer++;
    if (boss_fight_timer >= 160)
    {
        boss_fight_timer = 0;
        boss_x = rand() % 1300;
        boss_y = 128;
        iSetSpritePosition(&boss_i, boss_x, boss_y);
        iSetSpritePosition(&boss_r, boss_x, boss_y);
        iSetSpritePosition(&boss_a, boss_x, boss_y);
        iSetSpritePosition(&boss_d, boss_x, boss_y);
        iSetSpritePosition(&boss_ca, boss_x, boss_y);
    }

    // Mirroring logic
    if (soldier_position_x < boss_x && !bossMirrored)
    {
        iMirrorSprite(&boss_i, HORIZONTAL);
        iMirrorSprite(&boss_r, HORIZONTAL);
        iMirrorSprite(&boss_a, HORIZONTAL);
        iMirrorSprite(&boss_d, HORIZONTAL);
        iMirrorSprite(&boss_ca, HORIZONTAL);
        bossMirrored = true;
    }
    else if (soldier_position_x > boss_x + 120 && bossMirrored)
    {
        iMirrorSprite(&boss_i, HORIZONTAL);
        iMirrorSprite(&boss_r, HORIZONTAL);
        iMirrorSprite(&boss_a, HORIZONTAL);
        iMirrorSprite(&boss_d, HORIZONTAL);
        iMirrorSprite(&boss_ca, HORIZONTAL);
        bossMirrored = false;
    }

    static int bossCattackFrame = 0;
    static int bossAttackAnimFrame = 0;
    static int bossAttackTimer = 0;
    static bool bossAttackAnimPlaying = false;
    static bool bossFireMirrored = false;

    if (abs(boss_x - soldier_position_x) < 100)
    {
        // Close: run cattack only
        boss_state = 4;
        bossCattackFrame++;
        bossAttackAnimFrame = 0;
        bossAttackTimer = 0;
        bossAttackAnimPlaying = false;
        // Damage soldier every full cattack animation
        if (bossCattackFrame % 14 == 1 && !soldier_is_dead)
        {
            soldier_life -= 2;
            if (soldier_life <= 0)
            {
                soldier_is_dead = true;
            }
        }
    }
    else
    {
        // Far: throw fireball only
        bossCattackFrame = 0;
        boss_state = 2; // Attack animation (throw)
        bossAttackAnimFrame++;
        if (bossAttackAnimFrame >= 8)
        {
            bossAttackAnimFrame = 0;
            // Fireball spawn after attack animation
            if (!boss_fire_active)
            {
                boss_fire_active = true;
                boss_fire_x = boss_x + (bossMirrored ? -30 : 130);
                boss_fire_y = boss_y + 80;
                boss_fire_left = (soldier_position_x < boss_x);
                // Mirror fire sprite if needed
                if (boss_fire_left && !bossFireMirrored)
                {
                    iMirrorSprite(&boss_fire_sprite, HORIZONTAL);
                    bossFireMirrored = true;
                }
                else if (!boss_fire_left && bossFireMirrored)
                {
                    iMirrorSprite(&boss_fire_sprite, HORIZONTAL);
                    bossFireMirrored = false;
                }
            }
        }
        // Move boss toward soldier
        boss_state = 1;
        if (soldier_position_x > boss_x + 50)
        {
            boss_x += 6;
        }
        else if (soldier_position_x < boss_x)
        {
            boss_x -= 6;
        }
    }

    // Move fireball if active
    if (boss_fire_active)
    {
        if (boss_fire_left)
            boss_fire_x -= 10;
        else
            boss_fire_x += 10;
        iSetSpritePosition(&boss_fire_sprite, boss_fire_x, boss_fire_y);
        // Check collision with soldier
        if (checkCollision(boss_fire_x, boss_fire_y, 36, 36, soldier_position_x, soldier_position_y, 100, 100))
        {
            boss_fire_active = false;
            soldier_life -= 2;
            if (soldier_life <= 0)
            {
                soldier_is_dead = true;
            }
        }
        // Remove fireball if out of bounds
        if (boss_fire_x < -50 || boss_fire_x > 1500)
        {
            boss_fire_active = false;
        }
    }
    // Keep boss on ground
    boss_y = 128;
    iSetSpritePosition(&boss_i, boss_x, boss_y);
    iSetSpritePosition(&boss_r, boss_x, boss_y);
    iSetSpritePosition(&boss_a, boss_x, boss_y);
    iSetSpritePosition(&boss_d, boss_x, boss_y);
    iSetSpritePosition(&boss_ca, boss_x, boss_y);
}

void bullet_change_position()
{
    if (!isGameRunning)
        return;

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullet_fired[i])
        {
            // Move bullet
            if (bullet_direction[i])
            {
                bullet_positionx[i] -= 25; // left
                if (bullet_positionx[i] < -100)
                {
                    bullet_fired[i] = false;
                    bullet_positionx[i] = -100;
                    continue;
                }
            }
            else
            {
                bullet_positionx[i] += 25; // right
                if (bullet_positionx[i] > 1400)
                {
                    bullet_fired[i] = false;
                    bullet_positionx[i] = -100;
                    continue;
                }
            }
            // Boss hit detection (robust bounding box)
            if (boss_alive &&  !bullet_direction[i] && bullet_positionx[i] > boss_x+170 && soldier_position_x < boss_x+150 )
            {
                bullet_fired[i] = false;
                bullet_positionx[i] = -100;
                boss_health--;
                if (boss_health <= 0)
                {
                    boss_alive = false;
                    boss_health = boss_health_max;
                    isGameOver = true;
                    isGameRunning = false;
                    return;
                }
                continue;
            }
            if( boss_alive && bullet_direction[i] && bullet_positionx[i] < boss_x+180 && soldier_position_x > boss_x)
            {
                bullet_fired[i] = false;
                bullet_positionx[i] = -100;
                boss_health--;
                if (boss_health <= 0)
                {
                    boss_alive = false;
                    boss_health = boss_health_max;
                    isGameOver = true;
                    isGameRunning = false;
                    return;
                }
                continue;
            }
            // Zombie hit detection (robust bounding box)
            for (int j = 0; j < total_zombies; j++)
            {
                if (!zombie_dead_state[j] && checkCollision(bullet_positionx[i], bullet_positiony[i], 40, 40, zombie_position_x[j], zombie_position_y[j], 100, 100))
                {
                    bullet_fired[i] = false;
                    bullet_positionx[i] = -100;
                    zombie_dead_state[j] = true;
                    zombie_dead_frame_counter[j] = 0;
                    iSetSpritePosition(&zombie_d[j], zombie_position_x[j], zombie_position_y[j]);
                    break; // Bullet can only hit one thing per frame
                }
            }
        }
    }

    if (zombie_should_move)
    {
        for (int i = 0; i < total_zombies; i++)
        {
            if (!zombie_dead_state[i] && !zombie_attacking[i])
            {
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
    iSetTimer(50, bossUpdate); // Add this line to update boss logic
    // iSetTimer(100, zombieAttackUpdate);
    iInitialize(1400, 600, "Combined Game");
    return 0;
}