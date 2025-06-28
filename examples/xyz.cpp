#include "iGraphics.h"
#include "iSound.h"
#include <stdlib.h>
#include <time.h>

// flag for combine
bool isGameRunning = false;
bool isGameOver = false;
int gameOverTimer = 200000000;

// menu variables
#define BUTTON_COUNT 5
const char *menuItems[BUTTON_COUNT] = {"PLAY", "HIGH SCORE", "HELP", "CREDITS", "EXIT"};
int menuYStart = 350;
int menuX = 550;
int menuWidth = 150;
int menuHeight = 40;
int menuSpacing = 20;
int hoveredIndex = -1;
int soldier_life;
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
int zombie_life[MAX_ZOMBIES] = {0}; // Each zombie's life (hits left)

#define soldier 1
bool facing_r = true;
int bg_flag = 0;
bool is_running = false;
bool is_firing = false;
bool jump = false;
bool zombie_should_move = false;
bool left = false;  // Added left movement flag
bool right = false; // Added right movement flag
#define MAX_BULLETS 30
bool bullet_fired_r[MAX_BULLETS];
bool bullet_fired_l[MAX_BULLETS];
int bullet_position_r_x[MAX_BULLETS];
int bullet_position_r_y[MAX_BULLETS];
int bullet_position_l_x[MAX_BULLETS];
int bullet_position_l_y[MAX_BULLETS];
int soldier_position_x = 0;
int soldier_position_y = 128;

bool soldier_is_dying = false;
bool soldier_is_dead = false;
int soldier_death_frame = 0;

// Medicine variables
int medicine_x = 0, medicine_y = 128;
bool medicine_visible = false;
int medicine_timer = 0;
int medicine_respawn_time = 5000; // ms (5 seconds)
int maxLife = 10;                 // Make sure this matches your max soldier_life

// second_boss variables
Image second_boss_idle[8], second_boss_attack[7], second_boss_dead[10], second_boss_fire[11], second_boss_walk[8];
Sprite second_boss_spr_idle, second_boss_spr_attack, second_boss_spr_dead, second_boss_spr_fire, second_boss_spr_walk;
// Add cattack_sb sprite and frames
Image second_boss_cattack[11];
Sprite second_boss_spr_cattack;
int second_boss_x = 1000, second_boss_y = 128;
bool second_boss_alive = false;
bool second_boss_attacking = false;
int second_boss_life = 50; // second_boss needs 500 hits to die
int second_boss_attack_timer = 0, second_boss_attack_delay = 10;
bool second_boss_dead_animation_done = false;

int second_boss_dead_frame_counter = 0;

int second_boss_spawn_timer = 0;
bool second_boss_spawned = false;

// second_boss fire attack variables
bool second_boss_fire_active = false;
int second_boss_fire_x = 0, second_boss_fire_y = 0;
int second_boss_fire_speed = 50;
int second_boss_fire_cooldown = 0;
int second_boss_fire_cooldown_max = 20; // frames between fires

bool second_boss_attack_animating = false;
bool second_boss_fire_move = false;

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
            if (!checkCollision(zombie_position_x[i], zombie_position_y[i], 100, 100,
                                soldier_position_x, soldier_position_y, 100, 100))
            {
                zombie_attacking[i] = false;
                zombie_attack_counter[i] = 0;
                attack_frame_timer[i] = 0;
            }
        }
    }
}

void spawnZombies()
{
    total_zombies = 3 + rand() % (MAX_ZOMBIES - 3);
    for (int i = 0; i < total_zombies; i++)
    {
        zombie_position_x[i] = 1400 + rand() % 400 + i * 150;
        zombie_position_y[i] = 128;
        zombie_dead_state[i] = false;
        zombie_dead_animation_done[i] = false;
        zombie_dead_frame_counter[i] = 0;
        zombie_attacking[i] = false;
        zombie_attack_counter[i] = 0;
        attack_frame_timer[i] = 0;
        attack_frame_delay[i] = 7;
        zombie_life[i] = 5; // Each zombie needs 2 hits to die

        iInitSprite(&zombie_r[i]);
        iChangeSpriteFrames(&zombie_r[i], zombie_run, 7);
        iSetSpritePosition(&zombie_r[i], zombie_position_x[i], zombie_position_y[i]);
        iScaleSprite(&zombie_r[i], 3.75);

        iInitSprite(&zombie_d[i]);
        iChangeSpriteFrames(&zombie_d[i], zombie_dead, 5);
        iSetSpritePosition(&zombie_d[i], zombie_position_x[i], zombie_position_y[i]);
        iScaleSprite(&zombie_d[i], 3.75);

        iInitSprite(&zombie_a[i]);
        iChangeSpriteFrames(&zombie_a[i], zombie_attack, 5);
        iSetSpritePosition(&zombie_a[i], zombie_position_x[i], zombie_position_y[i]);
        iScaleSprite(&zombie_a[i], 3.75);
    }
}
bool allZombiesDead()
{
    for (int i = 0; i < total_zombies; i++)
    {
        if (!zombie_dead_state[i])
            return false;
    }
    return true;
}
void spawnZombiesPeriodically()
{
    if (allZombiesDead())

    {
        spawnZombies();
    }
}
void second_bossSpawnTimerUpdate()
{
    if (!second_boss_spawned && isGameRunning && !isGameOver)
    {
        second_boss_spawn_timer += 100;
        if (second_boss_spawn_timer >= 60000)
        { // 10 seconds, adjust as needed
            second_boss_alive = true;
            second_boss_spawned = true;
            second_boss_x = 1000;
            second_boss_life = 50;
            second_boss_dead_animation_done = false;
            iSetSpritePosition(&second_boss_spr_idle, second_boss_x, second_boss_y);
            iSetSpritePosition(&second_boss_spr_attack, second_boss_x, second_boss_y);
            iSetSpritePosition(&second_boss_spr_dead, second_boss_x, second_boss_y);
        }
    }
}
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
        bullet_fired_r[i] = false;
        bullet_fired_l[i] = false;
        bullet_position_r_x[i] = -100;
        bullet_position_r_y[i] = 250;
        bullet_position_l_x[i] = -100;
        bullet_position_l_y[i] = 250;
    }

    iLoadFramesFromFolder(zombie_run, "this_img/z_run");
    iLoadFramesFromFolder(zombie_dead, "this_img/z_dead");
    iLoadFramesFromFolder(zombie_attack, "this_img/z_attack");
    iSetTimer(5000, spawnZombiesPeriodically);
    iLoadFramesFromFolder(second_boss_idle, "this_img/idle_sb");
    iInitSprite(&second_boss_spr_idle);
    iChangeSpriteFrames(&second_boss_spr_idle, second_boss_idle, 8);
    iSetSpritePosition(&second_boss_spr_idle, second_boss_x, second_boss_y);
    iScaleSprite(&second_boss_spr_idle, 4);
    // No mirroring for second_boss

    iLoadFramesFromFolder(second_boss_attack, "this_img/attack_sb");
    iInitSprite(&second_boss_spr_attack);
    iChangeSpriteFrames(&second_boss_spr_attack, second_boss_attack, 7);
    iSetSpritePosition(&second_boss_spr_attack, second_boss_x, second_boss_y);
    iScaleSprite(&second_boss_spr_attack, 4);
    // No mirroring for second_boss

    iLoadFramesFromFolder(second_boss_dead, "this_img/dead_sb");
    iInitSprite(&second_boss_spr_dead);
    iChangeSpriteFrames(&second_boss_spr_dead, second_boss_dead, 10);
    iSetSpritePosition(&second_boss_spr_dead, second_boss_x, second_boss_y);
    iScaleSprite(&second_boss_spr_dead, 4);
    // No mirroring for second_boss

    iLoadFramesFromFolder(second_boss_fire, "this_img/fire_sb");
    iInitSprite(&second_boss_spr_fire);
    iChangeSpriteFrames(&second_boss_spr_fire, second_boss_fire, 11);
    iSetSpritePosition(&second_boss_spr_fire, second_boss_x, second_boss_y);
    iScaleSprite(&second_boss_spr_fire, 4);
    // No mirroring for second_boss

    iLoadFramesFromFolder(second_boss_walk, "this_img/walk_sb");
    iInitSprite(&second_boss_spr_walk);
    iChangeSpriteFrames(&second_boss_spr_walk, second_boss_walk, 8);
    iSetSpritePosition(&second_boss_spr_walk, second_boss_x, second_boss_y);
    iScaleSprite(&second_boss_spr_walk, 4);
    // No mirroring for second_boss

    // Load cattack_sb animation for second_boss close attack
    iLoadFramesFromFolder(second_boss_cattack, "this_img/cattack_sb");
    iInitSprite(&second_boss_spr_cattack);
    iChangeSpriteFrames(&second_boss_spr_cattack, second_boss_cattack, 11);
    iSetSpritePosition(&second_boss_spr_cattack, second_boss_x, second_boss_y);
    iScaleSprite(&second_boss_spr_cattack, 4);
    // No mirroring for second_boss

    iLoadImage(&menu_background, "assets/images/menu.png");
    iResizeImage(&menu_background, 1100, 550);

    iLoadImage(&gameover, "gameover.png");
    // iLoadImage(&gameover, "gameover1.png");
    iResizeImage(&gameover, 1100, 550);
    soldier_life = 10;
    isGameOver = false;
    gameOverTimer = 0;
}

void spawnMedicine()
{
    if (!medicine_visible)
    {
        medicine_x = 200 + rand() % 1000; // random x position
        medicine_y = 128;
        medicine_visible = true;
    }
}

void medicineTimerUpdate()
{
    if (!isGameRunning || isGameOver)
        return;
    medicine_timer += 100; // called every 100ms
    if (!medicine_visible && medicine_timer >= medicine_respawn_time)
    {
        spawnMedicine();
        medicine_timer = 0;
    }
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
        if (isGameRunning && !isGameOver)
        {
            // Green Life Bar
            int maxLife = 10; // Set this to your maximum soldier_life
            int barWidth = 200;
            int barHeight = 25;
            int x = 20, y = 560;

            // Draw border
            iSetColor(0, 0, 0);
            iRectangle(x - 2, y - 2, barWidth + 4, barHeight + 4);

            // Draw background (gray)
            iSetColor(100, 100, 100);
            iFilledRectangle(x, y, barWidth, barHeight);

            // Draw current life (green)
            iSetColor(0, 200, 0);
            iFilledRectangle(x, y, (int)(barWidth * ((float)soldier_life / maxLife)), barHeight);
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

        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullet_fired_r[i])
            {
                iSetSpritePosition(&bul, bullet_position_r_x[i], bullet_position_r_y[i]);
                iShowSprite(&bul);
            }
            if (bullet_fired_l[i])
            {
                iSetSpritePosition(&bul, bullet_position_l_x[i], bullet_position_l_y[i]);
                iShowSprite(&bul);
            }
        }

        // Draw medicine if visible
        if (medicine_visible)
        {
            iSetColor(255, 0, 0);
            iFilledRectangle(medicine_x, medicine_y, 30, 30);
            iSetColor(255, 255, 255);
            iFilledRectangle(medicine_x + 12, medicine_y + 5, 6, 20); // vertical bar
            iFilledRectangle(medicine_x + 5, medicine_y + 12, 20, 6); // horizontal bar
        }
        if (second_boss_alive && !second_boss_dead_animation_done)
        {
            if (second_boss_attack_animating)
            {
                iShowSprite(&second_boss_spr_attack); // Show attack sprite only when animating
            }
            else if (second_boss_fire_active)
            {
                iShowSprite(&second_boss_spr_idle); // Show idle while fireball is moving
            }
            else if (second_boss_x < soldier_position_x+10)
            {
                iShowSprite(&second_boss_spr_cattack); // Show close-attack animation when second_boss is left of soldier
            }
            else if (abs(second_boss_x - soldier_position_x) > 10)
            {
                iShowSprite(&second_boss_spr_walk); // Show walk animation when moving
            }
            else
            {
                iShowSprite(&second_boss_spr_idle); // Show idle when close (melee)
            }
        }
        else if (second_boss_spawned && !second_boss_alive && !second_boss_dead_animation_done)
        {
            iShowSprite(&second_boss_spr_dead);
        }
        // Draw second_boss fire
        if (second_boss_fire_active)
        {
            iSetSpritePosition(&second_boss_spr_fire, second_boss_fire_x, second_boss_fire_y);
            iShowSprite(&second_boss_spr_fire);
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
        left = false;
        right = false;
    }

    if (key == 'd')
    {
        is_running = true;
        is_firing = false;
        right = true;
        left = false;
        bg_flag = 1;
        zombie_should_move = true;
        // Only mirror if currently facing left
        if (soldier_r.flipHorizontal)
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL);
            facing_r = true;
        }
    }
    if (key == 'a')
    {
        is_firing = false;
        is_running = true;
        left = true;
        right = false;
        bg_flag = 2;
        // Only mirror if currently facing right
        if (!soldier_r.flipHorizontal)
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL);
            facing_r = false;
        }
    }
    if (key == 'f')
    {
        if (left)
        {
            if (facing_r)
            {
                iMirrorSprite(&soldier_fr, HORIZONTAL);
                facing_r = false;
            }
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (!bullet_fired_l[i])
                {
                    bullet_fired_l[i] = true;
                    bullet_position_l_x[i] = soldier_position_x - 40;
                    bullet_position_l_y[i] = 250;
                    break;
                }
            }
        }
        else if (right)
        {
            if (!facing_r)
            {
                iMirrorSprite(&soldier_fr, HORIZONTAL);
                facing_r = true;
            }
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (!bullet_fired_r[i])
                {
                    bullet_fired_r[i] = true;
                    bullet_position_r_x[i] = soldier_position_x + 150;
                    bullet_position_r_y[i] = 250;
                    break;
                }
            }
        }
        is_firing = true;
        is_running = false;
        zombie_should_move = true;
    }
    if (!left && !right && is_firing)
    {
        if (!facing_r)
        {
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            facing_r = true;
        }
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullet_fired_r[i])
            {
                bullet_fired_r[i] = true;
                bullet_position_r_x[i] = soldier_position_x + 150;
                bullet_position_r_y[i] = 250;
                break;
            }
        }
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
        left = true;
        right = false;
        bg_flag = 2;
        if (!soldier_r.flipHorizontal)
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL);
            facing_r = false;
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
        right = true;
        left = false;
        is_running = true;
        bg_flag = 1;
        if (soldier_r.flipHorizontal)
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL);
            facing_r = true;
        }
        if (soldier_position_x <= 1200)
        {
            for (int i = 0; i < total_zombies; i++)
            {
                if (checkCollision(zombie_position_x[i], zombie_position_y[i], 100, 100,
                                   soldier_position_x, soldier_position_y, 100, 100))
                {
                    return;
                }
            }
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
            { // 20 * 150ms = 3000ms = 3 sec
                isGameOver = true;
                isGameRunning = false;
            }
        }
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

                        if (zombie_attack_counter[i] >= 1)
                        {
                            zombie_attack_counter[i] = 0;
                            if (soldier_life > 0)
                                soldier_life -= 2;
                            if (soldier_life <= 0)
                            {
                                soldier_is_dead = true;
                            }
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
                    zombie_dead_frame_counter[i] = 0;
                    zombie_position_x[i] = 1400 + rand() % 400 + i * 150;
                    iSetSpritePosition(&zombie_r[i], zombie_position_x[i], zombie_position_y[i]);
                    iSetSpritePosition(&zombie_d[i], zombie_position_x[i], zombie_position_y[i]);
                    iSetSpritePosition(&zombie_a[i], zombie_position_x[i], zombie_position_y[i]);
                }
            }
        }

        // Check for medicine pickup
        if (medicine_visible && checkCollision(soldier_position_x, soldier_position_y, 100, 100, medicine_x, medicine_y, 30, 30))
        {
            if (soldier_life < maxLife)
                soldier_life++;
            medicine_visible = false;
            medicine_timer = 0;
        }
        // second_boss logic
        if (second_boss_alive && !second_boss_dead_animation_done)
        {
            if(second_boss_x < soldier_position_x ){
                second_boss_x = soldier_position_x ;
                iAnimateSprite(&second_boss_spr_cattack); 
            }
            // 1. second_boss always moves left, but never past the soldier
            if (second_boss_x > soldier_position_x + 10 && !second_boss_attack_animating && !second_boss_fire_active)
            {
                second_boss_attacking = false;
                iAnimateSprite(&second_boss_spr_walk);
                second_boss_x -= 10;
            }
            // 2. second_boss attack (melee)
            else if (!second_boss_attack_animating && !second_boss_fire_active)
            {
                second_boss_attacking = true;
                iAnimateSprite(&second_boss_spr_idle);
                second_boss_attack_timer++;
                if (second_boss_attack_timer >= second_boss_attack_delay)
                {
                    second_boss_attack_timer = 0;
                    if (soldier_life > 0)
                        soldier_life -= 4;
                    if (soldier_life <= 0)
                        soldier_is_dead = true;
                }
            }
            else
            {
                second_boss_attack_timer = 0;
            }
            // 3. second_boss fireball logic
            if (!second_boss_fire_active && !second_boss_attack_animating)
            {
                second_boss_fire_cooldown++;
                if (second_boss_fire_cooldown >= second_boss_fire_cooldown_max)
                {
                    second_boss_attack_animating = true;
                    second_boss_spr_attack.currentFrame = 0;
                    second_boss_fire_cooldown = 0;
                }
            }
            // 4. Animate attack and launch fireball at end of attack animation
            if (second_boss_attack_animating)
            {
                iAnimateSprite(&second_boss_spr_attack);
                if (second_boss_spr_attack.currentFrame == second_boss_spr_attack.totalFrames - 1)
                {
                    second_boss_attack_animating = false;
                    second_boss_fire_active = true;
                    second_boss_fire_x = second_boss_x;
                    second_boss_fire_y = second_boss_y + 80;
                }
            }
            // 5. Move fireball always left
            if (second_boss_fire_active)
            {
                second_boss_fire_x -= second_boss_fire_speed;
                // Only vanish fireball if it hits the soldier or leaves the window
                if (checkCollision(second_boss_fire_x, second_boss_fire_y, 30, 30, soldier_position_x, soldier_position_y, 30, 100))
                {
                    second_boss_fire_active = false;
                    if (soldier_life > 0)
                        soldier_life -= 3;
                    if (soldier_life <= 0)
                        soldier_is_dead = true;
                }
                else if (second_boss_fire_x < -100)
                {
                    second_boss_fire_active = false;
                }
            }
            // Sync all second_boss sprite positions
            iSetSpritePosition(&second_boss_spr_idle, second_boss_x, second_boss_y);
            iSetSpritePosition(&second_boss_spr_attack, second_boss_x, second_boss_y);
            iSetSpritePosition(&second_boss_spr_walk, second_boss_x, second_boss_y);
            iSetSpritePosition(&second_boss_spr_dead, second_boss_x, second_boss_y);
            iSetSpritePosition(&second_boss_spr_fire, second_boss_fire_x, second_boss_fire_y);
            iSetSpritePosition(&second_boss_spr_cattack, second_boss_x, second_boss_y); // Sync cattack sprite
            // 6. second_boss death check
            if (second_boss_life <= 0)
            {
                second_boss_alive = false;
                second_boss_attack_animating = false;
                second_boss_fire_active = false;
                second_boss_dead_frame_counter = 0;
            }
        }
        else if (second_boss_spawned && !second_boss_alive && !second_boss_dead_animation_done)
        {
            iAnimateSprite(&second_boss_spr_dead);
            second_boss_dead_frame_counter++;
            if (second_boss_dead_frame_counter >= 10)
            {
                second_boss_dead_animation_done = true;
            }
            second_boss_fire_active = false;
            if (second_boss_spawned && !second_boss_alive && second_boss_dead_animation_done)
            {
                second_boss_spawned = false;
                second_boss_spawn_timer = 0;
                // Move second_boss off-screen after death to prevent glimpse at old position
                second_boss_x = -500;
                iSetSpritePosition(&second_boss_spr_idle, second_boss_x, second_boss_y);
                iSetSpritePosition(&second_boss_spr_attack, second_boss_x, second_boss_y);
                iSetSpritePosition(&second_boss_spr_dead, second_boss_x, second_boss_y);
                iSetSpritePosition(&second_boss_spr_walk, second_boss_x, second_boss_y);
                iSetSpritePosition(&second_boss_spr_fire, second_boss_x, second_boss_y);
            }
        }
    }
}

void bullet_change_position()
{
    if (!isGameRunning)
        return;

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullet_fired_r[i])
        {
            bullet_position_r_x[i] += 25;
            if (bullet_position_r_x[i] > 1400)
            {
                bullet_fired_r[i] = false;
                bullet_position_r_x[i] = -100;
            }
            for (int j = 0; j < total_zombies; j++)
            {
                if (checkCollision(zombie_position_x[j], zombie_position_y[j], 100, 100,
                                   soldier_position_x, soldier_position_y, 100, 100) &&
                    is_firing && !zombie_dead_state[j])
                {
                    bullet_fired_r[i] = false;
                    bullet_position_r_x[i] = -100;
                    zombie_life[j]--;
                    if (zombie_life[j] <= 0)
                    {
                        zombie_dead_state[j] = true;
                        zombie_dead_frame_counter[j] = 0;
                        iSetSpritePosition(&zombie_d[j], zombie_position_x[j], zombie_position_y[j]);
                    }

                    break;
                }
            }
            for (int j = 0; j < total_zombies; j++)
            {
                if (!zombie_dead_state[j] && bullet_position_r_x[i] >= zombie_position_x[j] && bullet_position_r_x[i] <= zombie_position_x[j] + 100)
                {
                    bullet_fired_r[i] = false;
                    bullet_position_r_x[i] = -100;
                    zombie_life[j]--;
                    if (zombie_life[j] <= 0)
                    {
                        zombie_dead_state[j] = true;
                        zombie_dead_frame_counter[j] = 0;
                        iSetSpritePosition(&zombie_d[j], zombie_position_x[j], zombie_position_y[j]);
                    }
                }
            }
        }
        if (bullet_fired_l[i])
        {
            bullet_position_l_x[i] -= 25;
            if (bullet_position_l_x[i] < -50)
            {
                bullet_fired_l[i] = false;
                bullet_position_l_x[i] = -100;
            }
            for (int j = 0; j < total_zombies; j++)
            {
                if (checkCollision(zombie_position_x[j], zombie_position_y[j], 100, 100,
                                   soldier_position_x, soldier_position_y, 100, 100) &&
                    is_firing && !zombie_dead_state[j])
                {
                    bullet_fired_l[i] = false;
                    bullet_position_r_x[i] = -100;
                    zombie_life[j]--;
                    if (zombie_life[j] <= 0)
                    {
                        zombie_dead_state[j] = true;
                        zombie_dead_frame_counter[j] = 0;
                        iSetSpritePosition(&zombie_d[j], zombie_position_x[j], zombie_position_y[j]);
                    }

                    break;
                }
            }
            for (int j = 0; j < total_zombies; j++)
            {
                if (!zombie_dead_state[j] && bullet_position_l_x[i] >= zombie_position_x[j] && bullet_position_l_x[i] <= zombie_position_x[j] + 100)
                {
                    bullet_fired_l[i] = false;
                    bullet_position_l_x[i] = -100;
                    zombie_life[j]--;
                    if (zombie_life[j] <= 0)
                    {
                        zombie_dead_state[j] = true;
                        zombie_dead_frame_counter[j] = 0;
                        iSetSpritePosition(&zombie_d[j], zombie_position_x[j], zombie_position_y[j]);
                    }
                }
            }
        }
        // second_boss hit by right bullets
        if (second_boss_alive && !second_boss_dead_animation_done)
        {
            if (bullet_fired_r[i] && bullet_position_r_x[i] > second_boss_x + 150 && abs(bullet_position_r_x[i] - second_boss_x ) < 250)
            {
                bullet_fired_r[i] = false;
                bullet_position_r_x[i] = -100;
                second_boss_life -= 1; // Decrease second_boss life
            }
            // second_boss hit by left bullets
            if (bullet_fired_l[i] && bullet_position_l_x[i] < second_boss_x + 230 && abs(bullet_position_l_x[i] - second_boss_x) < 150)
            {
                bullet_fired_l[i] = false;
                bullet_position_l_x[i] = -100;
                second_boss_life -= 1; // Decrease second_boss life
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

void update_boss_state()
{
    if (!second_boss_alive || second_boss_life <= 0)
        return;
    // Determine if boss is moving toward soldier
    second_boss_attacking = false;
    if (abs(second_boss_x - soldier_position_x) > 200)
    {
        // Move boss toward soldier
        if (second_boss_x < soldier_position_x)
            second_boss_x += 5;
        else if (second_boss_x > soldier_position_x)
            second_boss_x -= 5;
    }
    else
    {
        second_boss_attacking = true;
        // Fireball logic or attack logic here
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
    iSetTimer(100, medicineTimerUpdate);
    iSetTimer(10, second_bossSpawnTimerUpdate);

    iInitialize(1400, 600, "Combined Game");
    return 0;
}