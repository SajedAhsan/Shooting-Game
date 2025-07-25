#include "iGraphics.h"
#include "iSound.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define MAX_LEADERBOARD_SIZE 5
#define MAX_NAME_LENGTH 20
#define MAX_ZOMBIES 10


int zombie_sound_channel = -1;
bool isZombieSoundPlaying = false;

// Structure for game state
typedef struct {
    bool valid;
    int soldier_life;
    int gameScore;
    int soldier_position_x;
    int soldier_position_y;
    int ammo_count;
    bool boss_phase;
    int second_boss_kill_count;
    char playerName[MAX_NAME_LENGTH];
    bool second_boss_alive;
    bool second_boss_spawned;
    int second_boss_life;
    bool facing_r;
    bool is_jumping;
    bool is_running;
    bool is_firing;
    bool zombie_should_move;
    // zombie state arrays
    int total_zombies;
    bool zombie_dead_state[MAX_ZOMBIES];
    bool zombie_dead_animation_done[MAX_ZOMBIES];
    int zombie_position_x[MAX_ZOMBIES];
    int zombie_position_y[MAX_ZOMBIES];
    int zombie_life[MAX_ZOMBIES];
} GameState;

// save state variables
GameState savedState = {false};
const char* SAVE_FILE = "saves/gamestate.bin";

// structure to store player score
typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
} PlayerScore;

// variables for leaderboard
PlayerScore leaderboard[MAX_LEADERBOARD_SIZE];
char currentPlayerName[MAX_NAME_LENGTH] = "";
bool isEnteringName = false;

// flags for game status
bool isGameRunning = false;
bool is_game_over = false;
int gameOverTimer = 0;
bool isVictory = false; // Flag for victory state
bool paused = false; // Pause flag

// menu variables
#define BUTTON_COUNT 6
const char *menu_items[BUTTON_COUNT] = {"PLAY", "RESUME", "LEADERBOARD", "HELP", "CREDITS", "EXIT"};
int menuYStart = 350;
int menuX = 550;
int menuWidth = 150;
int menuHeight = 40;
int menuSpacing = 20;
int hoveredIndex = -1;
int soldier_life = 60;
bool canResume = false;

int HIGH_SCORE = 0;
int gameScore = 0;
bool show_high_score_screen = false;
bool show_credits = false; 
bool show_help = false;
Image menu_background;


// game variables
Image bg;
Image gameover;
Image victory;
Image ammo;
Image credits;
Image help;

// Soldier variables
Image soldier_idle[1], soldier_run[8], soldier_jump[5], fire[3], bullet[1], soldier_dead[4];
Sprite soldier_i, soldier_r, soldier_j, soldier_fr, bul, soldier_d;
int soldier_position_x = 0;
int soldier_position_y = 128;
bool soldier_is_dying = false;
bool soldier_is_dead = false;
int soldier_death_frame = 0;


// Zombie variables
#define MAX_ZOMBIES 10
Image zombie_run[7], zombie_dead[5], zombie_attack[5];
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
int zombie_life[MAX_ZOMBIES] = {0}; 

//soldier flags
#define soldier 1
bool facing_r = true;
int bg_flag = 0;
bool is_running = false;
bool is_firing = false;
bool mouse_fire_held = false; 
bool jump = false;
bool is_jumping=false;
bool zombie_should_move = false;
bool left = false;  
bool right = false; 
int jump_height=200;
int jump_speed = 55; 
int jump_direction = 1; 
int ground_y = 128; 
int ground_level = 128; 
int max_jump_height = 200; 
// bullet varialbes
#define MAX_BULLETS 30
bool bullet_fired_r[MAX_BULLETS];
bool bullet_fired_l[MAX_BULLETS];
int bullet_position_r_x[MAX_BULLETS];
int bullet_position_r_y[MAX_BULLETS];
int bullet_position_l_x[MAX_BULLETS];
int bullet_position_l_y[MAX_BULLETS];

// medicine variables
int medicine_x = 0, medicine_y = 128;
bool medicine_visible = false;
int medicine_timer = 0;
int medicine_respawn_time = 5000; 
int maxLife = 60;  

//ammo variables
int ammo_count = 100; 
int ammo_max = 100; 
int ammo_respawn_time = 3000; 
int ammo_x= 0, ammo_y = 128; 
bool ammo_visible = false; 




// second_boss variables
Image second_boss_idle[8], second_boss_attack[7], second_boss_dead[10], second_boss_fire[11], second_boss_walk[8],second_boss_cattack[11];
Sprite second_boss_spr_idle, second_boss_spr_attack, second_boss_spr_dead, second_boss_spr_fire, second_boss_spr_walk,second_boss_spr_cattack;
int second_boss_x = 1100, second_boss_y = 128;
bool second_boss_alive = false;
bool second_boss_attacking = false;
int second_boss_life = 40; 
int second_boss_attack_timer = 0, second_boss_attack_delay = 10;
bool second_boss_dead_animation_done = false;
int second_boss_dead_frame_counter = 0;
int second_boss_spawn_timer = 0;
bool second_boss_spawned = false;
bool second_boss_fire_active = false;
int second_boss_fire_x = 0, second_boss_fire_y = 0;
int second_boss_fire_speed = 50;
int second_boss_fire_cooldown = 0;
int second_boss_fire_cooldown_max = 20; 
bool second_boss_attack_animating = false;
bool second_boss_fire_move = false;

// Boss sprite variables
Image boss_idle[7], boss_run[8], boss_attack[8], boss_dead[6], boss_cattack[14];
Sprite boss_i, boss_r, boss_a, boss_d, boss_ca;
int boss_x = 1100, boss_y = 128;
int boss_state = 0; // 0: idle, 1: running, 2: attacking, 3: dead 4: close attack
int boss_frame_timer = 0;
bool boss_alive = false;
int boss_health = 175;
int boss_health_max = 175;
bool bossMirrored = false;
// Boss fireball variables
Image boss_fire[11];
Sprite boss_fire_sprite;
bool boss_fire_active = false;
int boss_fire_x = -1000, boss_fire_y = -1000;
bool boss_fire_left = false;
// Tracking second boss kill count 
int second_boss_kill_count = 0;
bool boss_phase = false; 
bool waitingForRunAfterBoss = false;



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
// Function to reset all game state variables and positions for a fresh start
void resetGameState() {
    // clear any saved game state when starting a new game
    canResume = false;
    savedState.valid = false;
    // removed the save-state file
    remove(SAVE_FILE);
    
    // stop zombie sound if it's playing
    if (isZombieSoundPlaying) {
        iStopSound(zombie_sound_channel);
        isZombieSoundPlaying = false;
    }
    
    isGameRunning = true;
    is_game_over = false;
    isVictory = false;
    gameOverTimer = 0;
    soldier_life = 60;
    gameScore = 0;
    total_zombies = 0;
    medicine_visible = false;
    medicine_timer = 0;
    medicine_x = 0;
    medicine_y = 128;
    second_boss_spawned = false;
    second_boss_alive = false;
    second_boss_kill_count = 0;
    boss_phase = false;
    boss_alive = false;
    soldier_is_dead = false;
    soldier_position_x = 0;
    soldier_position_y = 128;
    ammo_count=60;
    ammo_visible = false;
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        zombie_dead_state[i] = false;
        zombie_dead_animation_done[i] = false;
        zombie_dead_frame_counter[i] = 0;
        zombie_position_x[i] = 0;
        zombie_position_y[i] = 128;
        zombie_attacking[i] = false;
        zombie_attack_counter[i] = 0;
        attack_frame_timer[i] = 0;
        attack_frame_delay[i] = 7;
        zombie_life[i] = 5;
    }
    boss_x = 1100;
    boss_y = 128;
    boss_health = boss_health_max;
    second_boss_x = 1100;
    second_boss_y = 128;
    second_boss_life = 40;
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullet_fired_r[i] = false;
        bullet_fired_l[i] = false;
        bullet_position_r_x[i] = -100;
        bullet_position_r_y[i] = 250;
        bullet_position_l_x[i] = -100;
        bullet_position_l_y[i] = 250;
    }
    is_running = false;
    is_firing = false;
    is_jumping = false;
    jump_direction = 1;
    iSetSpritePosition(&soldier_r, soldier_position_x, soldier_position_y);
    iSetSpritePosition(&soldier_i, soldier_position_x, soldier_position_y);
    iSetSpritePosition(&soldier_fr, soldier_position_x, soldier_position_y);
    iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
    iSetSpritePosition(&second_boss_spr_idle, second_boss_x, second_boss_y);
    iSetSpritePosition(&second_boss_spr_attack, second_boss_x, second_boss_y);
    iSetSpritePosition(&second_boss_spr_dead, second_boss_x, second_boss_y);
    iSetSpritePosition(&second_boss_spr_walk, second_boss_x, second_boss_y);
    iSetSpritePosition(&second_boss_spr_fire, second_boss_x, second_boss_y);
    iSetSpritePosition(&second_boss_spr_cattack, second_boss_x, second_boss_y);
    iSetSpritePosition(&boss_i, boss_x, boss_y);
    iSetSpritePosition(&boss_r, boss_x, boss_y);
    iSetSpritePosition(&boss_a, boss_x, boss_y);
    iSetSpritePosition(&boss_ca, boss_x, boss_y);
    iSetSpritePosition(&boss_d, boss_x, boss_y);
}
// helper to spawn ammo box near soldier
void spawnAmmoBox() {
    ammo_x = soldier_position_x + 70 + rand() % 200;
    if (ammo_x > 1200) ammo_x = 1200;
    ammo_y = 128;
    ammo_visible = true;
}
void spawnZombies()
{
    if (boss_phase || waitingForRunAfterBoss) return; 
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
        zombie_life[i] = 5; 

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
    
    //zombie sound is playing when spawning new zombies
    if (!isZombieSoundPlaying) {
        zombie_sound_channel = iPlaySound("assets/sounds/zombie.wav", true);
        isZombieSoundPlaying = true;
    }
}

// function to handle zombie sounds
void playZombieAmbience() {
    if (!isGameRunning || paused || is_game_over || boss_phase) {
        if (isZombieSoundPlaying) {
            iStopSound(zombie_sound_channel);
            isZombieSoundPlaying = false;
        }
        return;
    }
        
    // check if any zombies are visible on screen
    bool zombiesVisible = false;
    for (int i = 0; i < total_zombies; i++) {
        if (!zombie_dead_state[i] && zombie_position_x[i] >= -100 && zombie_position_x[i] <= 1400) {
            zombiesVisible = true;
            break;
        }
    }

    // handles zombie sounds based on visibility
    if (zombiesVisible && !isZombieSoundPlaying) {
        zombie_sound_channel = iPlaySound("assets/sounds/zombie.wav", true);
        isZombieSoundPlaying = true;
    } 
    else if (!zombiesVisible && isZombieSoundPlaying) {
        iStopSound(zombie_sound_channel);
        isZombieSoundPlaying = false;
    }
}
bool allZombiesDead()
{
    for (int i = 0; i < total_zombies; i++)
    {
        if (!zombie_dead_state[i] && zombie_position_x[i] >= -100)
            return false;
    }
    return true;
}
void spawnZombiesPeriodically()
{
    if (boss_phase || waitingForRunAfterBoss) return; 
    if (allZombiesDead())
    {
        spawnZombies();
    }
}
void second_bossSpawnTimerUpdate()
{
    if (boss_phase || waitingForRunAfterBoss || paused) return;
    if (!second_boss_spawned && isGameRunning && !is_game_over)
    {
        second_boss_spawn_timer += 100;
        if (second_boss_spawn_timer >= 40000)
        { 
            second_boss_alive = true;
            second_boss_spawned = true;
            second_boss_x = 1100;
            second_boss_life = 40;
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
    // loading menu background
    iLoadImage(&bg, "assets/bg/back.png");
    iResizeImage(&bg, 1400, 600);

    // credits
    iLoadImage(&credits, "assets/bg/credits.png");
    iResizeImage(&credits, 1400, 600);

    // help
    iLoadImage(&help, "assets/bg/help.png");
    iResizeImage(&help, 1400, 600);

    // loading sprties for soldier
    iLoadFramesFromFolder(soldier_run, "assets/this_img/run");
    iInitSprite(&soldier_r);
    iChangeSpriteFrames(&soldier_r, soldier_run, 8);
    iSetSpritePosition(&soldier_r, soldier_position_x, soldier_position_y);
    iScaleSprite(&soldier_r, 3.5);

    iLoadFramesFromFolder(soldier_idle, "assets/this_img/atack");
    iInitSprite(&soldier_i);
    iChangeSpriteFrames(&soldier_i, soldier_idle, 1);
    iSetSpritePosition(&soldier_i, soldier_position_x, soldier_position_y);
    iScaleSprite(&soldier_i, 3.5);

    iLoadFramesFromFolder(fire, "assets/this_img/attack");
    iInitSprite(&soldier_fr);
    iChangeSpriteFrames(&soldier_fr, fire, 3);
    iSetSpritePosition(&soldier_fr, soldier_position_x, soldier_position_y);
    iScaleSprite(&soldier_fr, 3.5);

    iLoadFramesFromFolder(soldier_dead, "assets/this_img/dead");
    iInitSprite(&soldier_d);
    iChangeSpriteFrames(&soldier_d, soldier_dead, 4);
    iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
    iScaleSprite(&soldier_d, 3.5);

    iLoadFramesFromFolder(bullet, "assets/this_img/bullet");
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

    // loading sprites for zombies
    iLoadFramesFromFolder(zombie_run, "assets/this_img/z_run");
    iLoadFramesFromFolder(zombie_dead, "assets/this_img/z_dead");
    iLoadFramesFromFolder(zombie_attack, "assets/this_img/z_attack");
    iSetTimer(9000, spawnZombiesPeriodically);

    // loading sprites for second_boss
    iLoadFramesFromFolder(second_boss_idle, "assets/this_img/idle_sb");
    iInitSprite(&second_boss_spr_idle);
    iChangeSpriteFrames(&second_boss_spr_idle, second_boss_idle, 8);
    iSetSpritePosition(&second_boss_spr_idle, second_boss_x, second_boss_y);
    iScaleSprite(&second_boss_spr_idle, 4);

    iLoadFramesFromFolder(second_boss_attack, "assets/this_img/attack_sb");
    iInitSprite(&second_boss_spr_attack);
    iChangeSpriteFrames(&second_boss_spr_attack, second_boss_attack, 7);
    iSetSpritePosition(&second_boss_spr_attack, second_boss_x, second_boss_y);
    iScaleSprite(&second_boss_spr_attack, 4);

    iLoadFramesFromFolder(second_boss_dead, "assets/this_img/dead_sb");
    iInitSprite(&second_boss_spr_dead);
    iChangeSpriteFrames(&second_boss_spr_dead, second_boss_dead, 10);
    iSetSpritePosition(&second_boss_spr_dead, second_boss_x, second_boss_y);
    iScaleSprite(&second_boss_spr_dead, 4);

    iLoadFramesFromFolder(second_boss_fire, "assets/this_img/fire_sb");
    iInitSprite(&second_boss_spr_fire);
    iChangeSpriteFrames(&second_boss_spr_fire, second_boss_fire, 11);
    iSetSpritePosition(&second_boss_spr_fire, second_boss_x, second_boss_y);
    iScaleSprite(&second_boss_spr_fire, 4);

    iLoadFramesFromFolder(second_boss_walk, "assets/this_img/walk_sb");
    iInitSprite(&second_boss_spr_walk);
    iChangeSpriteFrames(&second_boss_spr_walk, second_boss_walk, 8);
    iSetSpritePosition(&second_boss_spr_walk, second_boss_x, second_boss_y);
    iScaleSprite(&second_boss_spr_walk, 4);

    iLoadFramesFromFolder(second_boss_cattack, "assets/this_img/cattack_sb");
    iInitSprite(&second_boss_spr_cattack);
    iChangeSpriteFrames(&second_boss_spr_cattack, second_boss_cattack, 11);
    iSetSpritePosition(&second_boss_spr_cattack, second_boss_x, second_boss_y);
    iScaleSprite(&second_boss_spr_cattack, 4);

    // loading sprites for boss 
    iLoadFramesFromFolder(boss_idle, "assets/this_img/boss_idle");
    iInitSprite(&boss_i);
    iChangeSpriteFrames(&boss_i, boss_idle, 7);
    iSetSpritePosition(&boss_i, boss_x, boss_y);
    iScaleSprite(&boss_i, 4);

    iLoadFramesFromFolder(boss_run, "assets/this_img/boss_run");
    iInitSprite(&boss_r);
    iChangeSpriteFrames(&boss_r, boss_run, 8);
    iSetSpritePosition(&boss_r, boss_x, boss_y);
    iScaleSprite(&boss_r, 4);

    iLoadFramesFromFolder(boss_attack, "assets/this_img/boss_attack");
    iInitSprite(&boss_a);
    iChangeSpriteFrames(&boss_a, boss_attack, 8);
    iSetSpritePosition(&boss_a, boss_x, boss_y);
    iScaleSprite(&boss_a, 4);

    iLoadFramesFromFolder(boss_dead, "assets/this_img/boss_dead");
    iInitSprite(&boss_d);
    iChangeSpriteFrames(&boss_d, boss_dead, 6);
    iSetSpritePosition(&boss_d, boss_x, boss_y);
    iScaleSprite(&boss_d, 4);

    iLoadFramesFromFolder(boss_cattack, "assets/this_img/boss_cattack");
    iInitSprite(&boss_ca);
    iChangeSpriteFrames(&boss_ca, boss_cattack, 14);
    iSetSpritePosition(&boss_ca, boss_x, boss_y);
    iScaleSprite(&boss_ca, 4);

    iLoadFramesFromFolder(boss_fire, "assets/this_img/boss_fire");
    iInitSprite(&boss_fire_sprite);
    iChangeSpriteFrames(&boss_fire_sprite, boss_fire, 4);
    iScaleSprite(&boss_fire_sprite, 3);


    iLoadImage(&menu_background, "assets/bg/background.png");
    iResizeImage(&menu_background, 1400, 600);

    // loading gameover image
    iLoadImage(&gameover, "assets/bg/gameover.png");
    iResizeImage(&gameover, 1400, 600);
    // loading victory image
    iLoadImage(&victory, "assets/bg/victory.png");
    iResizeImage(&victory, 1400, 600);

    iLoadImage(&ammo, "assets/this_img/ammobox/ammo.png");
    iResizeImage(&ammo, 50, 50);

    soldier_life = 60;
    is_game_over = false;
    gameOverTimer = 0;

    boss_alive = false;
    boss_health = boss_health_max;
    boss_phase = false;
    second_boss_kill_count = 0;
}

// file path for leaderboard
const char* LEADERBOARD_FILE = "saves/leaderboard.txt";

void saveLeaderboard() {
    FILE* f = fopen(LEADERBOARD_FILE, "w");
    if (f) {
        for(int i = 0; i < MAX_LEADERBOARD_SIZE; i++) {
            fprintf(f, "%s %d\n", leaderboard[i].name, leaderboard[i].score);
        }
        fclose(f);
    }
}

void saveGameState() {
    if(!isGameRunning || is_game_over || isVictory) return;
    
    savedState.valid = true;
    savedState.soldier_life = soldier_life;
    savedState.gameScore = gameScore;
    savedState.soldier_position_x = soldier_position_x;
    savedState.soldier_position_y = soldier_position_y;
    savedState.ammo_count = ammo_count;
    savedState.boss_phase = boss_phase;
    savedState.second_boss_kill_count = second_boss_kill_count;
    savedState.second_boss_alive = second_boss_alive;
    savedState.second_boss_spawned = second_boss_spawned;
    savedState.second_boss_life = second_boss_life;
    strcpy(savedState.playerName, currentPlayerName);

    FILE* f = fopen(SAVE_FILE, "wb");
    if(f) {
        fwrite(&savedState, sizeof(GameState), 1, f);
        fclose(f);
    }
}

void loadGameState() {
    FILE* f = fopen(SAVE_FILE, "rb");
    if(f) {
        fread(&savedState, sizeof(GameState), 1, f);
        fclose(f);
        if(savedState.valid) {
            canResume = true;
        }
    }
}

void resumeGame() {
    if(!savedState.valid) return;
    
    isGameRunning = true;
    soldier_life = savedState.soldier_life;
    gameScore = savedState.gameScore;
    soldier_position_x = savedState.soldier_position_x;
    soldier_position_y = savedState.soldier_position_y;
    ammo_count = savedState.ammo_count;
    boss_phase = savedState.boss_phase;
    second_boss_kill_count = savedState.second_boss_kill_count;
    second_boss_alive = savedState.second_boss_alive;
    second_boss_spawned = savedState.second_boss_spawned;
    second_boss_life = savedState.second_boss_life;
    strcpy(currentPlayerName, savedState.playerName);

    // reset sprite positions
    iSetSpritePosition(&soldier_r, soldier_position_x, soldier_position_y);
    iSetSpritePosition(&soldier_i, soldier_position_x, soldier_position_y);
    iSetSpritePosition(&soldier_fr, soldier_position_x, soldier_position_y);
    iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
}

void cleanup() {
    saveGameState();
}

void loadLeaderboard() {
    FILE* f = fopen(LEADERBOARD_FILE, "r");
    if (f) {
        for(int i = 0; i < MAX_LEADERBOARD_SIZE; i++) {
            fscanf(f, "%s %d", leaderboard[i].name, &leaderboard[i].score);
        }
        fclose(f);
    } else {
        // Initialize empty leaderboard
        for(int i = 0; i < MAX_LEADERBOARD_SIZE; i++) {
            strcpy(leaderboard[i].name, "---");
            leaderboard[i].score = 0;
        }
        saveLeaderboard(); // Create initial file
    }
}

void updateLeaderboard(const char* name, int score) {
    // if player already exists
    int existingPos = -1;
    for(int i = 0; i < MAX_LEADERBOARD_SIZE; i++) {
        if(strcmp(leaderboard[i].name, name) == 0) {
            existingPos = i;
            break;
        }
    }

    // player exists and new score is better, remove old entry
    if(existingPos != -1) {
        if(score <= leaderboard[existingPos].score) {
            return;
        }
        // remove existing entry
        for(int i = existingPos; i < MAX_LEADERBOARD_SIZE-1; i++) {
            strcpy(leaderboard[i].name, leaderboard[i+1].name);
            leaderboard[i].score = leaderboard[i+1].score;
        }
        strcpy(leaderboard[MAX_LEADERBOARD_SIZE-1].name, "---");
        leaderboard[MAX_LEADERBOARD_SIZE-1].score = 0;
    }

    int insertPos = -1;
    for(int i = 0; i < MAX_LEADERBOARD_SIZE; i++) {
        if(strcmp(leaderboard[i].name, "---") == 0 || score > leaderboard[i].score) {
            insertPos = i;
            break;
        }
    }

    if(insertPos == -1) {
        // Score is too low to make it to top 5
        return;
    }

    // Shift scores down to make room for new score
    for(int i = MAX_LEADERBOARD_SIZE-1; i > insertPos; i--) {
        strcpy(leaderboard[i].name, leaderboard[i-1].name);
        leaderboard[i].score = leaderboard[i-1].score;
    }

    // Insert new score
    strcpy(leaderboard[insertPos].name, name);
    leaderboard[insertPos].score = score;

    saveLeaderboard();
}

void spawnMedicine()
{
    if (!medicine_visible && !boss_phase)
    {
        medicine_x = 200 + rand() % 1000; // random x position
        medicine_y = 128;
        medicine_visible = true;
    }
}

void medicineTimerUpdate()
{
    if (!isGameRunning || is_game_over || paused)
        return;
    medicine_timer += 150; 
    if (!medicine_visible && medicine_timer >= medicine_respawn_time && !boss_phase)
    {
        spawnMedicine();
        medicine_timer = 0;
    }

}
void updateJump() {
    if (!is_jumping) return;
    soldier_position_y += jump_speed * jump_direction;
    soldier_i.y += jump_speed * jump_direction;
    soldier_r.y += jump_speed * jump_direction;
    soldier_fr.y += jump_speed * jump_direction;
    soldier_d.y += jump_speed * jump_direction;

    if (soldier_position_y >= ground_y + jump_height) {
        jump_direction = -1;
    } else if (soldier_position_y <= ground_y) {
        soldier_position_y = ground_y;
        soldier_i.y = ground_y;
        soldier_r.y = ground_y;
        soldier_fr.y = ground_y;
        soldier_d.y = ground_y;
        is_jumping = false;
        jump_direction = 1;
    }
    zombie_should_move = true;
}

void iDraw()
{
    iClear();

    if (is_game_over) {
        if (gameScore > 0) {
            updateLeaderboard(currentPlayerName, gameScore);
        }
        // clear saved game when game is over
        canResume = false;
        savedState.valid = false;
        remove(SAVE_FILE);
        
        if (isVictory) {
            iShowLoadedImage(50, 50, &victory);
        } else {
            iShowLoadedImage(50, 50, &gameover);
        }
        iSetColor(255, 255, 0);
        iText(600, 80, "Press 'h' to return main menu", GLUT_BITMAP_HELVETICA_18);
        iSetColor(255, 255, 255);
        return;
    }

    if (!isGameRunning)
    {
        iShowLoadedImage(0, 0, &menu_background);
        if(isEnteringName) {
            iSetColor(255, 255, 0);
            iText(550, 400, "Enter your name:", GLUT_BITMAP_HELVETICA_18);
            iSetColor(255, 255, 255);
            iText(550, 350, currentPlayerName, GLUT_BITMAP_HELVETICA_18);
            iText(550, 300, "Press ENTER to start", GLUT_BITMAP_HELVETICA_12);
            return;
        }
        if (show_high_score_screen) {
            iSetColor(255, 255, 0);
            iText(600, 500, "LEADERBOARD", GLUT_BITMAP_HELVETICA_18);
            for(int i = 0; i < MAX_LEADERBOARD_SIZE; i++) {
                char scoreText[64];
                sprintf(scoreText, "%d. %s: %d", i+1, leaderboard[i].name, leaderboard[i].score);
                iText(600, 450-i*40, scoreText, GLUT_BITMAP_HELVETICA_18);
            }
            iSetColor(255,255,255);
            iText(600, 200, "Press ESC to return", GLUT_BITMAP_HELVETICA_12);
            return;
        }
        if(show_credits)
        {
            iShowLoadedImage(0, 0, &credits);
            return;
        }
        if(show_help)
        {
            iShowLoadedImage(0, 0, &help);
            return;
        }
        int visibleButtons = 0;
        for (int i = 0; i < BUTTON_COUNT; i++) {
            if (i == 1 && !canResume) continue;
            visibleButtons++;
        }

        int adjustedMenuYStart = menuYStart + ((BUTTON_COUNT - visibleButtons) * (menuHeight + menuSpacing)) / 2;
        

        int current_visible_index = 0;
        for (int i = 0; i < BUTTON_COUNT; i++)
        {

            if (i == 1 && !canResume) continue;

            int y = adjustedMenuYStart - current_visible_index * (menuHeight + menuSpacing);
            
            if (i == hoveredIndex)
                iSetColor(20, 250, 235);
            else if (i == 1 && !canResume)
                iSetColor(100, 100, 100); 
            else
                iSetColor(255, 255, 255);
                
            iText(menuX, y, menu_items[i], GLUT_BITMAP_HELVETICA_18);
            current_visible_index++;
        }
    }
    else
    {

        iShowLoadedImage(0, 0, &bg);
        if (paused) {
            iSetColor(255, 255, 0);
            iText(600, 300, "PAUSED", GLUT_BITMAP_HELVETICA_18);
            iSetColor(255, 255, 255);
            iText(570, 260, "Press 'r' to resume", GLUT_BITMAP_HELVETICA_12);
            return;
        }

        if (isGameRunning && !is_game_over)
        {
            
            int maxLife = 60; 
            int barWidth = 200;
            int barHeight = 25;
            int x = 20, y = 560;

            // draw border
            iSetColor(0, 0, 0);
            iRectangle(x - 2, y - 2, barWidth + 4, barHeight + 4);

            // draw background
            iSetColor(100, 100, 100);
            iFilledRectangle(x, y, barWidth, barHeight);

            // draw current life 
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
            gameScore += 1; 

            iShowSprite(&soldier_r);
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


        if (medicine_visible)
        {
            iSetColor(255, 0, 0);
            iFilledRectangle(medicine_x, medicine_y, 30, 30);
            iSetColor(255, 255, 255);
            iFilledRectangle(medicine_x + 12, medicine_y + 5, 6, 20); 
            iFilledRectangle(medicine_x + 5, medicine_y + 12, 20, 6); 
        }
        if (second_boss_alive && !second_boss_dead_animation_done)
        {
            if (second_boss_attack_animating)
            {
                iShowSprite(&second_boss_spr_attack); 
            }
            else if (second_boss_fire_active)
            {
                iShowSprite(&second_boss_spr_idle); 
            }
            else if (second_boss_x < soldier_position_x+10)
            {
                iShowSprite(&second_boss_spr_cattack); 
            }
            else if (abs(second_boss_x - soldier_position_x) > 10)
            {
                iShowSprite(&second_boss_spr_walk); 
            }
            else
            {
                iShowSprite(&second_boss_spr_idle);
            }
        }
        else if (second_boss_spawned && !second_boss_alive && !second_boss_dead_animation_done)
        {
            iShowSprite(&second_boss_spr_dead);
        }

        if (second_boss_fire_active)
        {
            iSetSpritePosition(&second_boss_spr_fire, second_boss_fire_x, second_boss_fire_y);
            iShowSprite(&second_boss_spr_fire);
        }

 
        if (boss_phase && boss_alive)
        {
            if (boss_state == 0)
                iShowSprite(&boss_i);
            else if (boss_state == 1)
                iShowSprite(&boss_r);
            else if (boss_state == 2)
                iShowSprite(&boss_a);
            else if (boss_state == 4)
                iShowSprite(&boss_ca);
            if (boss_fire_active)
            {
                iShowSprite(&boss_fire_sprite);
            }
            iSetColor(200, 0, 0);
            iFilledRectangle(1100, 560, 200 * boss_health / boss_health_max, 25);
            iSetColor(255, 255, 255);
            iRectangle(1100, 560, 200, 25);
            iSetColor(0, 0, 0);
            iText(1110, 573, "Boss HP", GLUT_BITMAP_HELVETICA_12);
            iSetColor(255, 255, 255);
        }
        // display the score in yellow
        char scoreText[32];
        sprintf(scoreText, "SCORE : %d", gameScore);
        iSetColor(255, 255, 0);
        iText(600, 570, scoreText, GLUT_BITMAP_HELVETICA_18);
        if (waitingForRunAfterBoss) {
            iSetColor(255, 0, 0);
            iText(1100, 300, "GO GO -->", GLUT_BITMAP_HELVETICA_18);
        }

        // draw ammo count at top left
        char ammoText[32];
        sprintf(ammoText, "AMMO : %d", ammo_count);
        iSetColor(0, 200, 255);
        iText(20, 530, ammoText, GLUT_BITMAP_HELVETICA_18);
        // draw ammo box if visible
        if (ammo_visible) {
            iShowLoadedImage(ammo_x, ammo_y, &ammo);
        }
    }
}

void iMouseMove(int mx, int my)
{
    if (!isGameRunning && !isEnteringName && !show_high_score_screen && !show_credits && !show_help)
    {
        hoveredIndex = -1;
        
        int visibleButtons = 0;
        for (int i = 0; i < BUTTON_COUNT; i++) {
            if (i == 1 && !canResume) continue;
            visibleButtons++;
        }
        
        int adjustedMenuYStart = menuYStart + ((BUTTON_COUNT - visibleButtons) * (menuHeight + menuSpacing)) / 2;
        
        int current_visible_index = 0;
        
        for (int i = 0; i < BUTTON_COUNT; i++)
        {
            // skip the RESUME button if we can't resume
            if (i == 1 && !canResume) continue;
            
            int y = adjustedMenuYStart - current_visible_index * (menuHeight + menuSpacing);
            
            // check if mouse is over this button
            if (mx >= menuX - 10 && mx <= menuX + menuWidth + 10 && 
                my >= y - menuHeight/2 && my <= y + menuHeight/2)
            {
                hoveredIndex = i;
                break;
            }
            
            current_visible_index++;
        }
    }
}

void iMouse(int button, int state, int mx, int my)
{
    if (!isGameRunning && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && hoveredIndex != -1)
    {
        iPlaySound("assets/sounds/clicked.wav", false);
        if (hoveredIndex == 0) // PLAY
        {
            isEnteringName = true;
            currentPlayerName[0] = '\0';
        }
        else if (hoveredIndex == 1) // RESUME
        {
            if (canResume)
            {
                resumeGame();
            }
        }
        else if (hoveredIndex == 2) // LEADERBOARD
        {
            show_high_score_screen = true;
        }
        else if (hoveredIndex == 3) // HELP
        {
            show_help = true;
        }
        else if (hoveredIndex == 4) // CREDITS
        {
            show_credits = true;
        }
        else if (hoveredIndex == 5) // EXIT
        {
            exit(0);
        }
    }

    if (isGameRunning) {
        if (button == GLUT_LEFT_BUTTON && (state == GLUT_DOWN || state == GLUT_HOLD)) {
            if (is_jumping) return; 
            mouse_fire_held = true;
            is_firing = true;
            is_running = false;
            if (ammo_count > 0) {
                ammo_count--;
                if (left) {
                    if (!soldier_fr.flipHorizontal) {
                        iMirrorSprite(&soldier_fr, HORIZONTAL);
                    }
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullet_fired_l[i]) {
                            bullet_fired_l[i] = true;
                            bullet_position_l_x[i] = soldier_position_x - 40;
                            bullet_position_l_y[i] = 250;
                            break;
                        }
                    }
                } else if (right) {
                    if (soldier_fr.flipHorizontal) {
                        iMirrorSprite(&soldier_fr, HORIZONTAL);
                    }
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullet_fired_r[i]) {
                            bullet_fired_r[i] = true;
                            bullet_position_r_x[i] = soldier_position_x + 150;
                            bullet_position_r_y[i] = 250;
                            break;
                        }
                    }
                } else {
                    if (!facing_r) {
                        iMirrorSprite(&soldier_fr, HORIZONTAL);
                        facing_r = true;
                    }
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullet_fired_r[i]) {
                            bullet_fired_r[i] = true;
                            bullet_position_r_x[i] = soldier_position_x + 150;
                            bullet_position_r_y[i] = 250;
                            break;
                        }
                    }
                }
                zombie_should_move = true;
                if (ammo_count == 0 && !ammo_visible) {
                    spawnAmmoBox();
                }
            }
        }
        if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
            mouse_fire_held = false;
            is_firing = false;
            is_running = false;
        }
    }
}
void iMouseDrag(int mx, int my) {};
void iMouseWheel(int dir, int mx, int my) {};

void iKeyboard(unsigned char key, int state)
{
    if(isEnteringName) {
        if(state == GLUT_DOWN) {
            if(key == '\r') { 
                if(strlen(currentPlayerName) > 0) {
                    isEnteringName = false;
                    resetGameState();
                }
                return;
            }
            else if(key == 8) { 
                int len = strlen(currentPlayerName);
                if(len > 0) currentPlayerName[len-1] = '\0';
            }
            else if(strlen(currentPlayerName) < MAX_NAME_LENGTH - 1) {
                if((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || 
                   (key >= '0' && key <= '9') || key == ' ') {
                    int len = strlen(currentPlayerName);
                    currentPlayerName[len] = key;
                    currentPlayerName[len+1] = '\0';
                }
            }
            return;
        }
        return;
    }

    if (key == 'p' && state == GLUT_DOWN) {
        paused = true;
        return;
    }
    if (key == 'r' && state == GLUT_DOWN) {
        paused = false;
        return;
    }

    if ((is_game_over || isVictory) && key == 'h' && state == GLUT_DOWN) {
        isGameRunning = false;
        is_game_over = false;
        isVictory = false;
        show_high_score_screen = false;
        show_credits = false;
        show_help = false;
        return;
    }

    if (!isGameRunning)
    {
        if (show_high_score_screen && key == 27) { 
            show_high_score_screen = false;
        }
        if (show_credits && key == 27) { 
            show_credits = false;
        }
        if (show_help && key == 27) { 
            show_help = false;
        }
        return;
    }

    if (state == GLUT_UP) {
        is_running = false;
        bg_flag = 0;
        left = false;
        right = false;
        iSetSpritePosition(&soldier_i, soldier_position_x, soldier_position_y);
        return;
    }

    if (key == 's' && state == GLUT_DOWN)
    { 
        is_running = false;
        is_firing = false;
        left = false;
        right = false;
    }

    if (key == 'd' && state == GLUT_DOWN)
    {
        is_running = true;
        is_firing = false;
        right = true;
        left = false;
        bg_flag = 1;
        zombie_should_move = true;
        facing_r = true;
        if (soldier_r.flipHorizontal)
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL);
            
        }
        if (waitingForRunAfterBoss) {
            waitingForRunAfterBoss = false;
        }
    }
    if (key == 'a' && state == GLUT_DOWN)
    {
        is_firing = false;
        is_running = true;
        left = true;
        right = false;
        bg_flag = 2;
        facing_r = false;
        if (!soldier_r.flipHorizontal)
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL); 
        }
    }
    // if (key == 'f') {
    //     if (is_jumping) return;
    //     if (state == GLUT_DOWN) {
    //         is_firing = true;
    //         is_running = false;
    //         if (ammo_count > 0) {
    //             ammo_count--;
    //             if (left) {
    //                 if (!soldier_fr.flipHorizontal) {
    //                     iMirrorSprite(&soldier_fr, HORIZONTAL);
    //                 }
    //                 for (int i = 0; i < MAX_BULLETS; i++) {
    //                     if (!bullet_fired_l[i]) {
    //                         bullet_fired_l[i] = true;
    //                         bullet_position_l_x[i] = soldier_position_x - 40;
    //                         bullet_position_l_y[i] = 250;
                            
    //                         break;
    //                     }
    //                 }
    //             } else if (right) {
    //                 if (soldier_fr.flipHorizontal) {
    //                     iMirrorSprite(&soldier_fr, HORIZONTAL);
    //                 }
    //                 for (int i = 0; i < MAX_BULLETS; i++) {
    //                     if (!bullet_fired_r[i]) {
    //                         bullet_fired_r[i] = true;
    //                         bullet_position_r_x[i] = soldier_position_x + 150;
    //                         bullet_position_r_y[i] = 250;
    //                         break;
    //                     }
    //                 }
    //             } else {
    //                 if (!facing_r) {
    //                     iMirrorSprite(&soldier_fr, HORIZONTAL);
    //                     facing_r = true;
    //                 }
    //                 for (int i = 0; i < MAX_BULLETS; i++) {
    //                     if (!bullet_fired_r[i]) {
    //                         bullet_fired_r[i] = true;
    //                         bullet_position_r_x[i] = soldier_position_x + 150;
    //                         bullet_position_r_y[i] = 250;
                            
    //                         break;
    //                     }
    //                 }
    //             }
    //             zombie_should_move = true;
    //             if (ammo_count == 0 && !ammo_visible) {
    //                 spawnAmmoBox();
    //             }
    //         }
    //     } else if (state == GLUT_UP) {
    //         is_firing = false;
    //         is_running = false;
    //         iSetSpritePosition(&soldier_i, soldier_position_x, soldier_position_y);
    //     }
    //     return;
    // }
    if (key == 'w' && !is_jumping) {
        is_jumping = true;
        is_running = false;
        is_firing = false;
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

void iSpecialKeyboard(int key, int state)
{
    if (!isGameRunning)
    {
        return;
    }

    if (key == GLUT_KEY_END)
    {
        exit(0);
    }
    if(key == GLUT_KEY_HOME)
    {
        isGameRunning = false;
        is_game_over = false;
        gameOverTimer = 0;
        soldier_life = 60;
        gameScore = 0;
        total_zombies = 0;
        medicine_visible = false;
        medicine_timer = 0;
        medicine_x = 0;
        medicine_y = 128;
        second_boss_spawned = false;
        second_boss_alive = false;
        second_boss_kill_count = 0;
        boss_phase = false; 
        boss_alive = false;
        return;
    }
    if (!isGameRunning)
    {
        return;
    }

    if (key == GLUT_KEY_END)
    {
        exit(0);
    }

    if (state == GLUT_UP) {
        is_running = false;
        bg_flag = 0;
        left = false;
        right = false;
        iSetSpritePosition(&soldier_i, soldier_position_x, soldier_position_y);
        return;
    }

    if (key == GLUT_KEY_LEFT && state == GLUT_DOWN)
    {
        left = true;
        right = false;
        bg_flag = 2;
        is_running = true;
        if (!soldier_r.flipHorizontal)
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL);
            facing_r = false;
        }
    }

    if (key == GLUT_KEY_RIGHT && state == GLUT_DOWN)
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
        if (waitingForRunAfterBoss) {
            waitingForRunAfterBoss = false;
        }
    }
    if (key == GLUT_KEY_DOWN && state == GLUT_DOWN)
    {
        iKeyboard('s',GLUT_DOWN);
    }
    if (key == GLUT_KEY_UP && state == GLUT_DOWN)
    {
        iKeyboard('w',GLUT_DOWN);
    }
}



void boss_update() {
    if (!boss_phase || !boss_alive || is_game_over) return;
    bool should_mirror = (soldier_position_x < boss_x);
    if (should_mirror != bossMirrored) {
        iMirrorSprite(&boss_i, HORIZONTAL);
        iMirrorSprite(&boss_r, HORIZONTAL);
        iMirrorSprite(&boss_a, HORIZONTAL);
        iMirrorSprite(&boss_ca, HORIZONTAL);
        iMirrorSprite(&boss_d, HORIZONTAL);
        bossMirrored = should_mirror;
    }
    int boss_speed = 24;
    int boss_cattack_range = 100;
    static int boss_cattack_frame = 0;
    static int boss_attack_anim_frame = 0;
    static bool boss_fire_mirrored = false;
    static bool boss_attack_anim_playing = false;
    static bool fireball_queued = false;

    if (abs(boss_x - soldier_position_x) < boss_cattack_range) {
        boss_state = 4; 
        boss_cattack_frame++;
        boss_attack_anim_frame = 0;
        boss_attack_anim_playing = false;
        fireball_queued = false;
        if (boss_cattack_frame % 14 == 1 && !soldier_is_dead) {
            if (soldier_life > 0) soldier_life -= 5;
            if (soldier_life <= 0) soldier_is_dead = true;
        }
    } else {
        boss_cattack_frame = 0;
        if (!boss_attack_anim_playing && !boss_fire_active) {
            boss_attack_anim_playing = true;
            boss_attack_anim_frame = 0;
            fireball_queued = false;
        }
        if (boss_attack_anim_playing) {
            boss_state = 2; 
            boss_attack_anim_frame += 2; 
            if (boss_attack_anim_frame >= 8) {
                boss_attack_anim_playing = false;
                boss_attack_anim_frame = 0;
                fireball_queued = true;
            }
        } else {
            boss_state = 1; 
            if (soldier_position_x > boss_x + 50) {
                boss_x += boss_speed;
            } else if (soldier_position_x < boss_x) {
                boss_x -= boss_speed;
            }
        }

        if (fireball_queued && !boss_fire_active) {
            boss_fire_active = true;
            boss_fire_x = boss_x + (bossMirrored ? -30 : 130);
            boss_fire_y = boss_y + 80;
            boss_fire_left = (soldier_position_x < boss_x);
            if (boss_fire_left && !boss_fire_mirrored) {
                iMirrorSprite(&boss_fire_sprite, HORIZONTAL);
                boss_fire_mirrored = true;
            } else if (!boss_fire_left && boss_fire_mirrored) {
                iMirrorSprite(&boss_fire_sprite, HORIZONTAL);
                boss_fire_mirrored = false;
            }
            fireball_queued = false;
        }
    }

    if (boss_fire_active) {
        int fire_speed = 40;
        if (boss_fire_left) boss_fire_x -= fire_speed;
        else boss_fire_x += fire_speed;
        iSetSpritePosition(&boss_fire_sprite, boss_fire_x, boss_fire_y);
        if (checkCollision(boss_fire_x, boss_fire_y, 40, 40, soldier_position_x, soldier_position_y, 100, 100)) {
            boss_fire_active = false;
            boss_fire_x = -1000;
            if (soldier_life > 0) soldier_life -= 7;
            if (soldier_life <= 0) soldier_is_dead = true;
        } else if (boss_fire_x < -100 || boss_fire_x > 1500) {
            boss_fire_active = false;
            boss_fire_x = -1000;
        }
    }

    iSetSpritePosition(&boss_i, boss_x, boss_y);
    iSetSpritePosition(&boss_r, boss_x, boss_y);
    iSetSpritePosition(&boss_a, boss_x, boss_y);
    iSetSpritePosition(&boss_ca, boss_x, boss_y);
    iSetSpritePosition(&boss_d, boss_x, boss_y);
}


void iAnim()
{
    if (!isGameRunning || paused)
        return;
    updateJump();
    if (!soldier_is_dead)
    {
        iAnimateSprite(&soldier_i);
        if (is_running && !is_jumping)
        {
            int screen_middle = 430;
            if (!(boss_phase && boss_alive) && !second_boss_alive) {
                if (right) {
                    bool blocked = false;
                    for (int i = 0; i < total_zombies; i++) {
                        if (checkCollision(zombie_position_x[i], zombie_position_y[i], 100, 100,
                                           soldier_position_x, soldier_position_y, 100, 100)) {
                            blocked = true;
                            break;
                        }
                    }
                    if (!blocked) {
                        if (soldier_position_x < screen_middle) {
                            int move_dist = 22;
                            if (soldier_position_x + move_dist > screen_middle)
                                move_dist = screen_middle - soldier_position_x;
                            soldier_position_x += move_dist;
                            soldier_r.x += move_dist;
                            soldier_i.x += move_dist;
                            soldier_fr.x += move_dist;
                        } else if (soldier_position_x == screen_middle) {
                            iWrapImage(&bg, -22);
                            if (medicine_visible) {
                                medicine_x -= 22;
                            }
                            if (ammo_visible) {
                                ammo_x -= 22;
                            }
                        }
                        if (soldier_position_x > screen_middle) {
                            int diff = soldier_position_x - screen_middle;
                            soldier_position_x = screen_middle;
                            soldier_r.x -= diff;
                            soldier_i.x -= diff;
                            soldier_fr.x -= diff;
                        }
                        zombie_should_move = true;
                        gameScore += 1;
                    }
                }
                if (left && soldier_position_x > 0) {
                    int move_dist = 22;
                    if (soldier_position_x - move_dist < 0)
                        move_dist = soldier_position_x;
                    soldier_position_x -= move_dist;
                    soldier_r.x -= move_dist;
                    soldier_i.x -= move_dist;
                    soldier_fr.x -= move_dist;
                    gameScore += 1;
                }
            } else {
                if (right && soldier_position_x <= 1200) {
                    bool blocked = false;
                    for (int i = 0; i < total_zombies; i++) {
                        if (checkCollision(zombie_position_x[i], zombie_position_y[i], 100, 100,
                                           soldier_position_x, soldier_position_y, 100, 100)) {
                            blocked = true;
                            break;
                        }
                    }
                    if (!blocked) {
                        soldier_position_x += 30;
                        soldier_r.x += 30;
                        soldier_i.x += 30;
                        soldier_fr.x += 30;
                        zombie_should_move = true;
                        gameScore += 1;
                    }
                }
                if (left && soldier_position_x != 0) {
                    soldier_position_x -= 22;
                    soldier_r.x -= 22;
                    soldier_i.x -= 22;
                    soldier_fr.x -= 22;
                    gameScore += 1;
                }
            }
            if (left && soldier_position_x != 0) {
                soldier_position_x -= 22;
                soldier_r.x -= 22;
                soldier_i.x -= 22;
                soldier_fr.x -= 22;
                gameScore += 1;
            }
            iAnimateSprite(&soldier_r);
        }
        static int mouse_fire_delay = 0;
        if (mouse_fire_held && !is_jumping && ammo_count > 0) {
            mouse_fire_delay++;
            if (mouse_fire_delay >= 1) {
                ammo_count--;
                if (facing_r) {
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullet_fired_r[i]) {
                            bullet_fired_r[i] = true;
                            bullet_position_r_x[i] = soldier_position_x + 150;
                            bullet_position_r_y[i] = 250;
                            iPlaySound("assets/sounds/shoot.wav", false);
                            break;
                        }
                    }
                } else {
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullet_fired_l[i]) {
                            bullet_fired_l[i] = true;
                            bullet_position_l_x[i] = soldier_position_x - 40;
                            bullet_position_l_y[i] = 250;
                            iPlaySound("assets/sounds/shoot.wav", false);
                            break;
                        }
                    }
                }
                zombie_should_move = true;
                if (ammo_count == 0 && !ammo_visible) {
                    spawnAmmoBox();
                }
                mouse_fire_delay = 0;
            }
        } else {
            mouse_fire_delay = 2;
        }
        static int f_key_fire_delay = 0;
        if (is_firing && !is_jumping && ammo_count > 0) {
            f_key_fire_delay++;
            if (f_key_fire_delay >= 1) {
                ammo_count--;
                if (left) {
                    if (facing_r) {
                        iMirrorSprite(&soldier_fr, HORIZONTAL);
                        facing_r = false;
                    }
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullet_fired_l[i]) {
                            bullet_fired_l[i] = true;
                            bullet_position_l_x[i] = soldier_position_x - 40;
                            bullet_position_l_y[i] = 250;
                            iPlaySound("assets/sounds/shoot.wav", false);
                            break;
                        }
                    }
                } else if (right) {
                    if (!facing_r) {
                        iMirrorSprite(&soldier_fr, HORIZONTAL);
                        facing_r = true;
                    }
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullet_fired_r[i]) {
                            bullet_fired_r[i] = true;
                            bullet_position_r_x[i] = soldier_position_x + 150;
                            bullet_position_r_y[i] = 250;
                            iPlaySound("assets/sounds/shoot.wav", false);
                            break;
                        }
                    }
                } else {
                    if (!facing_r) {
                        iMirrorSprite(&soldier_fr, HORIZONTAL);
                        facing_r = true;
                    }
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullet_fired_r[i]) {
                            bullet_fired_r[i] = true;
                            bullet_position_r_x[i] = soldier_position_x + 150;
                            bullet_position_r_y[i] = 250;
                            iPlaySound("assets/sounds/shoot.wav", false);
                            break;
                        }
                    }
                }
                zombie_should_move = true;
                if (ammo_count == 0 && !ammo_visible) {
                    spawnAmmoBox();
                }
                f_key_fire_delay = 0;
            }
        } else {
            f_key_fire_delay = 2;
        }
        if (!is_firing && !is_jumping) {
            iSetSpritePosition(&soldier_i, soldier_position_x, soldier_position_y);
        }
        if (is_firing && !is_jumping)
            iAnimateSprite(&soldier_fr);
    }
    else
    {
        iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
        iAnimateSprite(&soldier_d);
        soldier_death_frame++;
        if (soldier_death_frame >= 4)
        {
            gameOverTimer++;
            if (gameOverTimer >= 10)
            {
                is_game_over = true;
                isGameRunning = false;
            }
        }
    }

    if (boss_phase && boss_alive && !is_game_over)
    {
        boss_update(); 
        if (boss_state == 0)
            iAnimateSprite(&boss_i);
        else if (boss_state == 1)
            iAnimateSprite(&boss_r);
        else if (boss_state == 2)
            iAnimateSprite(&boss_a);
        else if (boss_state == 4)                                
            iAnimateSprite(&boss_ca);
    }

    if (!is_game_over && !boss_phase)
    {
        for (int i = 0; i < total_zombies; i++)
        {
            if (!zombie_dead_state[i])
            {
                if (checkCollision(zombie_position_x[i], zombie_position_y[i], 100, 100,
                                   soldier_position_x, soldier_position_y, 100, 100))
                {
                    gameScore+=10;
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
                                soldier_life -= 4;
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

        if (medicine_visible && checkCollision(soldier_position_x, soldier_position_y, 100, 100, medicine_x, medicine_y, 30, 30))
        {
            if (soldier_life < maxLife)
                soldier_life++;
            medicine_visible = false;
            medicine_timer = 0;
        }
   
        if (second_boss_alive && !second_boss_dead_animation_done)
        {
            if(second_boss_x < soldier_position_x ){
                second_boss_x = soldier_position_x ;
                iAnimateSprite(&second_boss_spr_cattack); 
            }

            if (second_boss_x > soldier_position_x + 10 && !second_boss_attack_animating && !second_boss_fire_active)
            {
                second_boss_attacking = false;
                iAnimateSprite(&second_boss_spr_walk);
                second_boss_x -= 10;
            }

            else if (!second_boss_attack_animating && !second_boss_fire_active)
            {
                second_boss_attacking = true;
                iAnimateSprite(&second_boss_spr_idle);
                second_boss_attack_timer++;
                if (second_boss_attack_timer >= second_boss_attack_delay)
                {
                    second_boss_attack_timer = 0;
                    if (soldier_life > 0)
                        soldier_life -= 7;
                    if (soldier_life <= 0)
                        soldier_is_dead = true;
                }
            }
            else
            {
                second_boss_attack_timer = 0;
            }
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
            if (second_boss_fire_active)
            {
                second_boss_fire_x -= second_boss_fire_speed;
                if (checkCollision(second_boss_fire_x, second_boss_fire_y, 30, 30, soldier_position_x, soldier_position_y, 30, 100))
                {
                    second_boss_fire_active = false;
                    if (soldier_life > 0)
                        soldier_life -= 9;
                    if (soldier_life <= 0)
                        soldier_is_dead = true;
                }
                else if (second_boss_fire_x < -100)
                {
                    second_boss_fire_active = false;
                }
            }

            iSetSpritePosition(&second_boss_spr_idle, second_boss_x, second_boss_y);
            iSetSpritePosition(&second_boss_spr_attack, second_boss_x, second_boss_y);
            iSetSpritePosition(&second_boss_spr_walk, second_boss_x, second_boss_y);
            iSetSpritePosition(&second_boss_spr_dead, second_boss_x, second_boss_y);
            iSetSpritePosition(&second_boss_spr_fire, second_boss_x, second_boss_y);
            iSetSpritePosition(&second_boss_spr_cattack, second_boss_x, second_boss_y); 

            if (second_boss_life <= 0)
            {
                if (second_boss_alive) {
                    gameScore += 500; 
                    second_boss_alive = false;
                    second_boss_attack_animating = false;
                    second_boss_fire_active = false;
                    second_boss_dead_frame_counter = 0;
                    second_boss_kill_count++;
                    waitingForRunAfterBoss = true; 
                    if (second_boss_kill_count >= 5 && !boss_phase) {
                        boss_phase = true;
                        boss_alive = true;
                        boss_health = boss_health_max;
                        medicine_visible = false;
                        for (int i = 0; i < total_zombies; i++) {
                            zombie_dead_state[i] = true;
                            zombie_dead_animation_done[i] = true;
                            zombie_position_x[i] = -1000;
                        }
                        second_boss_x = -1000;
                        iSetSpritePosition(&second_boss_spr_idle, second_boss_x, second_boss_y);
                        iSetSpritePosition(&second_boss_spr_attack, second_boss_x, second_boss_y);
                        iSetSpritePosition(&second_boss_spr_dead, second_boss_x, second_boss_y);
                        iSetSpritePosition(&second_boss_spr_walk, second_boss_x, second_boss_y);
                        iSetSpritePosition(&second_boss_spr_fire, second_boss_x, second_boss_y);
                        boss_x = 1100;
                        boss_y = 128;
                        boss_state = 0;
                        boss_frame_timer = 0;
                        iSetSpritePosition(&boss_i, boss_x, boss_y);
                        iSetSpritePosition(&boss_r, boss_x, boss_y);
                        iSetSpritePosition(&boss_a, boss_x, boss_y);
                        iSetSpritePosition(&boss_ca, boss_x, boss_y);
                        iSetSpritePosition(&boss_d, boss_x, boss_y);
                    }
                }
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
    if (!isGameRunning || paused)
        return;
    if (boss_phase && boss_alive)
    {
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
                if (boss_alive &&  bullet_position_r_x[i] > boss_x+170 && soldier_position_x < boss_x+150 )
                {
                    bullet_fired_r[i] = false;
                    bullet_position_r_x[i] = -100;
                    boss_health--;
                    if (boss_health <= 0)
                    {
                        gameScore += 1000; 
                        boss_alive = false;
                        isVictory = true;
                        is_game_over = true;
                        isGameRunning = false;
                        return;
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
                if (boss_alive && bullet_position_l_x[i] < boss_x+180 && soldier_position_x > boss_x)
                {
                    bullet_fired_l[i] = false;
                    bullet_position_l_x[i] = -100;
                    boss_health--;
                    if (boss_health <= 0)
                    {
                        gameScore += 1000; 
                        boss_alive = false;
                        isVictory = true;
                        is_game_over = true;
                        isGameRunning = false;
                        return;
                    }
                }
            }
        }
        return;
    }
 
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
                        gameScore += 10; 
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
                        gameScore += 10; 
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
                    bullet_position_l_x[i] = -100;
                    zombie_life[j]--;
                    if (zombie_life[j] <= 0)
                    {
                        zombie_dead_state[j] = true;
                        gameScore += 10; 
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
                        gameScore += 10; 
                        zombie_dead_frame_counter[j] = 0;
                        iSetSpritePosition(&zombie_d[j], zombie_position_x[j], zombie_position_y[j]);
                    }
                }
            }
        }
        if (second_boss_alive && !second_boss_dead_animation_done)
        {
            if (bullet_fired_r[i] && bullet_position_r_x[i] > second_boss_x + 150 && abs(bullet_position_r_x[i] - second_boss_x ) < 250)
            {
                bullet_fired_r[i] = false;
                bullet_position_r_x[i] = -100;
                second_boss_life -= 1; 
            }

            if (bullet_fired_l[i] && bullet_position_l_x[i] < second_boss_x + 230 && abs(bullet_position_l_x[i] - second_boss_x) < 150)
            {
                bullet_fired_l[i] = false;
                bullet_position_l_x[i] = -100;
                second_boss_life -= 1; 
            }
            if (second_boss_life <= 0 && second_boss_alive) {
                gameScore += 500;
                second_boss_alive = false;
                second_boss_attack_animating = false;
                second_boss_fire_active = false;
                second_boss_dead_frame_counter = 0;
                second_boss_kill_count++;
                waitingForRunAfterBoss = true; 

                if (second_boss_kill_count >= 5 && !boss_phase) {
                    boss_phase = true;
                    boss_alive = true;
                    boss_health = boss_health_max;
                    medicine_visible = false;

                    for (int z = 0; z < total_zombies; z++) {
                        zombie_dead_state[z] = true;
                        zombie_dead_animation_done[z] = true;
                    }
                    total_zombies = 0;
                    second_boss_x = -500;
                    iSetSpritePosition(&second_boss_spr_idle, second_boss_x, second_boss_y);
                    iSetSpritePosition(&second_boss_spr_attack, second_boss_x, second_boss_y);
                    iSetSpritePosition(&second_boss_spr_dead, second_boss_x, second_boss_y);
                    iSetSpritePosition(&second_boss_spr_walk, second_boss_x, second_boss_y);
                    iSetSpritePosition(&second_boss_spr_fire, second_boss_x, second_boss_y);
                    iSetSpritePosition(&second_boss_spr_cattack, second_boss_x, second_boss_y);

                    boss_x = 1100;
                    boss_y = 128;
                    boss_state = 0;
                    boss_frame_timer = 0;
                    bossMirrored = false;
                    iSetSpritePosition(&boss_i, boss_x, boss_y);
                    iSetSpritePosition(&boss_r, boss_x, boss_y);
                    iSetSpritePosition(&boss_a, boss_x, boss_y);
                    iSetSpritePosition(&boss_ca, boss_x, boss_y);
                    iSetSpritePosition(&boss_d, boss_x, boss_y);
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

void checkAmmoPickup() {
    if (!isGameRunning || paused || is_game_over)
        return;
        
    if (ammo_visible && checkCollision(soldier_position_x, soldier_position_y, 100, 100, ammo_x, ammo_y, 50, 50)) {
        ammo_count = 100;
        ammo_visible = false;
    }
}



void update_boss_state()
{
    if (!second_boss_alive || second_boss_life <= 0){
        
        gameScore += 500;
        return; 
    }

    second_boss_attacking = false;
    if (abs(second_boss_x - soldier_position_x) > 200)
    {
        if (second_boss_x < soldier_position_x)
            second_boss_x += 5;
        else if (second_boss_x > soldier_position_x)
            second_boss_x -= 5;
    }
    else
    {
        second_boss_attacking = true;
    }
}



int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    loadLeaderboard();
    loadGameState(); 
    loadResources();
    iInitializeSound();
    atexit(cleanup);
    glutCloseFunc(cleanup);
    iSetTimer(150, iAnim);
    iSetTimer(50, bullet_change_position);
    iSetTimer(100, zombieAttackUpdate);
    iSetTimer(100, medicineTimerUpdate);
    iSetTimer(10, second_bossSpawnTimerUpdate);
    iSetTimer(1, playZombieAmbience);
    iSetTimer(50, checkAmmoPickup);
    iOpenWindow(1400, 600, "Combined Game");
    return 0;
}