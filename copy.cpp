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
bool is_zombie_sound_playing = false;
#define MAX_BULLETS 30
// Structure for game state
typedef struct
{
    bool valid;
    int soldier_life;
    int game_Score;
    int soldier_position_x;
    int soldier_position_y;
    int ammo_count;
    bool boss_phase;
    int second_boss_kill_count;
    int zombie_wave_count;
    char player_name[MAX_NAME_LENGTH];
    bool second_boss_alive;
    bool second_boss_spawned;
    int second_boss_life;
    bool facing_r;
    bool is_jumping;
    bool is_running;
    bool is_firing;
    bool zombie_should_move;
    
    // Boss state variables
    bool boss_alive;
    int boss_health;
    int boss_x;
    int boss_y;
    int boss_state;
    bool waitingForRunAfterBoss;
    bool bossMirrored;
    
    // Second boss additional state
    int second_boss_x;
    int second_boss_y;
    bool second_boss_attacking;
    int second_boss_attack_timer;
    bool second_boss_dead_animation_done;
    int second_boss_dead_frame_counter;
    int second_boss_spawn_timer;
    bool second_boss_fire_active;
    int second_boss_fire_x;
    int second_boss_fire_y;
    int second_boss_fire_cooldown;
    bool second_boss_attack_animating;
    
    // Boss fire state
    bool boss_fire_active;
    int boss_fire_x;
    int boss_fire_y;
    bool boss_fire_left;
    
    // Medicine and ammo state
    bool medicine_visible;
    int medicine_x;
    int medicine_y;
    int medicine_timer;
    bool ammo_visible;
    int ammo_x;
    int ammo_y;
    
    // zombie state arrays
    int total_zombies;
    bool zombie_dead_state[MAX_ZOMBIES];
    bool zombie_dead_animation_done[MAX_ZOMBIES];
    int zombie_position_x[MAX_ZOMBIES];
    int zombie_position_y[MAX_ZOMBIES];
    int zombie_life[MAX_ZOMBIES];
    bool zombie_attacking[MAX_ZOMBIES];
    int zombie_attack_counter[MAX_ZOMBIES];
    int zombie_dead_frame_counter[MAX_ZOMBIES];
    
    // ADD THESE MISSING STATES:
    
    // Missing zombie arrays
    int attack_frame_delay[MAX_ZOMBIES];
    int attack_frame_timer[MAX_ZOMBIES];
    
    // Missing soldier states
    bool soldier_is_dying;
    bool soldier_is_dead;
    int soldier_death_frame;
    
    // Missing movement flags
    bool left;
    bool right;
    int bg_flag;
    
    // Missing jump states
    bool jump;
    int jump_direction;
    
    // Missing bullet arrays
    bool bullet_fired_r[MAX_BULLETS];
    bool bullet_fired_l[MAX_BULLETS];
    int bullet_position_r_x[MAX_BULLETS];
    int bullet_position_r_y[MAX_BULLETS];
    int bullet_position_l_x[MAX_BULLETS];
    int bullet_position_l_y[MAX_BULLETS];
    
    // Missing input states
    bool mouse_fire_held;
    
    // Missing boss frame timer
    int boss_frame_timer;
    
    // Missing second boss delay
    int second_boss_attack_delay;
    
} GameState;

// save state variables
GameState saved_state = {false};
const char *SAVE_FILE = "saves/gamestate.bin";

// structure to store player score
typedef struct
{
    char name[MAX_NAME_LENGTH];
    int score;
} PlayerScore;

// variables for leaderboard
PlayerScore leaderboard[MAX_LEADERBOARD_SIZE];
char current_player_name[MAX_NAME_LENGTH] = "";
bool is_entering_name = false;

// flags for game status
bool is_game_running = false;
bool is_game_over = false;
int game_over_timer = 0;
bool is_victory = false;
bool paused = false;

// settings variables
bool zombie_sound_enabled = true;
bool bullet_sound_enabled = true;
bool show_settings = false;

// menu variables
#define BUTTON_COUNT 7
const char *menu_items[BUTTON_COUNT] = {"PLAY", "RESUME", "LEADERBOARD", "SETTINGS", "HELP", "CREDITS", "EXIT"};
int menu_y_start = 350;
int menu_x = 550;
int menu_width = 150;
int menu_height = 40;
int menu_spacing = 20;
int hovered_index = -1;
int soldier_life = 200;
bool can_resume = false;

int HIGH_SCORE = 0;
int game_Score = 0;
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

// zombie variables
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

// soldier flags
#define soldier 1
bool facing_r = true;
int bg_flag = 0;
bool is_running = false;
bool is_firing = false;
bool mouse_fire_held = false;
bool jump = false;
bool is_jumping = false;
bool zombie_should_move = false;
bool left = false;
bool right = false;
int jump_height = 200;
int jump_speed = 55;
int jump_direction = 1;
int ground_y = 128;
int ground_level = 128;
int max_jump_height = 200;

// bullet varialbes

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
int maxLife = 200;

// ammo variables
int ammo_count = 100;
int ammo_max = 100;
int ammo_respawn_time = 3000;
int ammo_x = 0, ammo_y = 128;
bool ammo_visible = false;

// second_boss variables
Image second_boss_idle[8], second_boss_attack[7], second_boss_dead[10], second_boss_fire[11], second_boss_walk[8], second_boss_cattack[11];
Sprite second_boss_spr_idle, second_boss_spr_attack, second_boss_spr_dead, second_boss_spr_fire, second_boss_spr_walk, second_boss_spr_cattack;
int second_boss_x = 900, second_boss_y = 128;
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

// boss sprite variables
Image boss_idle[7], boss_run[8], boss_attack[8], boss_dead[6], boss_cattack[14];
Sprite boss_i, boss_r, boss_a, boss_d, boss_ca;
int boss_x = 950, boss_y = 128;
int boss_state = 0; // 0: idle, 1: running, 2: attacking, 3: dead 4: close attack
int boss_frame_timer = 0;
bool boss_alive = false;
int boss_health = 175;
int boss_health_max = 175;
bool bossMirrored = false;

// boss fireball variables
Image boss_fire[11];
Sprite boss_fire_sprite;
bool boss_fire_active = false;
int boss_fire_x = -1000, boss_fire_y = -1000;
bool boss_fire_left = false;

// tracking second boss kill count
int second_boss_kill_count = 0;
int zombie_wave_count = 0; // Track zombie waves (4 waves before each second boss)
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
// function to reset all game state variables and positions for a fresh start
void resetGameState()
{
    // clear any saved game state when starting a new game
    can_resume = false;
    saved_state.valid = false;
    remove(SAVE_FILE);

    // stop zombie sound if it's playing
    if (is_zombie_sound_playing)
    {
        iStopSound(zombie_sound_channel);
        is_zombie_sound_playing = false;
    }

    is_game_running = true;
    is_game_over = false;
    is_victory = false;
    game_over_timer = 0;
    soldier_life = 200;
    game_Score = 0;
    total_zombies = 0;
    medicine_visible = false;
    medicine_timer = 0;
    medicine_x = 0;
    medicine_y = 128;
    second_boss_spawned = false;
    second_boss_alive = false;
    second_boss_kill_count = 0;
    zombie_wave_count = 0;
    boss_phase = false;
    boss_alive = false;
    soldier_is_dead = false;
    soldier_position_x = 0;
    soldier_position_y = 128;
    ammo_count = 60;
    ammo_visible = false;
    for (int i = 0; i < MAX_ZOMBIES; i++)
    {
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
    boss_x = 950;
    boss_y = 128;
    boss_health = boss_health_max;
    second_boss_x = 900;
    second_boss_y = 128;
    second_boss_life = 40;
    for (int i = 0; i < MAX_BULLETS; i++)
    {
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
void spawnAmmoBox()
{
    ammo_x = soldier_position_x + 70 + rand() % 200;
    if (ammo_x > 1200)
        ammo_x = 1200;
    ammo_y = 128;
    ammo_visible = true;
}
void spawnZombies()
{
    if (boss_phase || waitingForRunAfterBoss)
        return;
    total_zombies = 5 + rand() % (MAX_ZOMBIES - 3);
    for (int i = 0; i < total_zombies; i++)
    {
        zombie_position_x[i] = 1200 + rand() % 400 + i * 150;
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

    // zombie sound is playing when spawning new zombies
    if (!is_zombie_sound_playing && zombie_sound_enabled)
    {
        zombie_sound_channel = iPlaySound("assets/sounds/zombie.wav", true);
        is_zombie_sound_playing = true;
    }
}

// function to handle zombie sounds
void playZombieAmbience()
{
    if (!is_game_running || paused || is_game_over || boss_phase || !zombie_sound_enabled)
    {
        if (is_zombie_sound_playing)
        {
            iStopSound(zombie_sound_channel);
            is_zombie_sound_playing = false;
        }
        return;
    }

    // check if any zombies are visible on screen
    bool zombiesVisible = false;
    for (int i = 0; i < total_zombies; i++)
    {
        if (!zombie_dead_state[i] && zombie_position_x[i] >= -100 && zombie_position_x[i] <= 1200)
        {
            zombiesVisible = true;
            break;
        }
    }

    // handles zombie sounds based on visibility
    if (zombiesVisible && !is_zombie_sound_playing)
    {
        zombie_sound_channel = iPlaySound("assets/sounds/zombie.wav", true);
        is_zombie_sound_playing = true;
    }
    else if (!zombiesVisible && is_zombie_sound_playing)
    {
        iStopSound(zombie_sound_channel);
        is_zombie_sound_playing = false;
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
    if (boss_phase || waitingForRunAfterBoss)
        return;
    if (allZombiesDead())
    {
        // Only spawn more zombies if we haven't reached 4 waves yet
        if (zombie_wave_count < 4)
        {
            zombie_wave_count++;
            spawnZombies();
        }
        // After 4 waves, let the second boss spawn
    }
}
void second_bossSpawnTimerUpdate()
{
    if (boss_phase || waitingForRunAfterBoss || paused)
        return;
    if (!second_boss_spawned && is_game_running && !is_game_over)
    {
        // Spawn second boss after every 4 zombie waves
        if (zombie_wave_count >= 4 && allZombiesDead())
        {
            second_boss_alive = true;
            second_boss_spawned = true;
            second_boss_x = 900;
            second_boss_life = 40;
            second_boss_dead_animation_done = false;
            zombie_wave_count = 0; // Reset wave count for next cycle
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
    iResizeImage(&bg, 1200, 600);

    // credits
    iLoadImage(&credits, "assets/bg/credits.png");
    iResizeImage(&credits, 1200, 600);

    // help
    iLoadImage(&help, "assets/bg/help.png");
    iResizeImage(&help, 1200, 600);

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
    iSetTimer(4000, spawnZombiesPeriodically);

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
    iResizeImage(&menu_background, 1200, 600);

    // loading gameover image
    iLoadImage(&gameover, "assets/bg/gameover.png");
    iResizeImage(&gameover, 1200, 600);
    // loading victory image
    iLoadImage(&victory, "assets/bg/victory.png");
    iResizeImage(&victory, 1200, 600);

    iLoadImage(&ammo, "assets/this_img/ammobox/ammo.png");
    iResizeImage(&ammo, 50, 50);

    soldier_life = 200;
    is_game_over = false;
    game_over_timer = 0;

    boss_alive = false;
    boss_health = boss_health_max;
    boss_phase = false;
    second_boss_kill_count = 0;
    zombie_wave_count = 0;
}

// file path for leaderboard
const char *LEADERBOARD_FILE = "saves/leaderboard.txt";

void saveLeaderboard()
{
    FILE *f = fopen(LEADERBOARD_FILE, "w");
    if (f)
    {
        for (int i = 0; i < MAX_LEADERBOARD_SIZE; i++)
        {
            fprintf(f, "%s %d\n", leaderboard[i].name, leaderboard[i].score);
        }
        fclose(f);
    }
}

void saveGameState()
{
    if (is_game_over || is_victory)
        return;

    saved_state.valid = true;
    saved_state.soldier_life = soldier_life;
    saved_state.game_Score = game_Score;
    saved_state.soldier_position_x = soldier_position_x;
    saved_state.soldier_position_y = soldier_position_y;
    saved_state.ammo_count = ammo_count;
    saved_state.boss_phase = boss_phase;
    saved_state.second_boss_kill_count = second_boss_kill_count;
    saved_state.zombie_wave_count = zombie_wave_count;
    saved_state.second_boss_alive = second_boss_alive;
    saved_state.second_boss_spawned = second_boss_spawned;
    saved_state.second_boss_life = second_boss_life;
    strcpy(saved_state.player_name, current_player_name);
    
    // Save missing soldier states
    saved_state.facing_r = facing_r;
    saved_state.is_jumping = is_jumping;
    saved_state.is_running = is_running;
    saved_state.is_firing = is_firing;
    saved_state.zombie_should_move = zombie_should_move;
    saved_state.soldier_is_dying = soldier_is_dying;
    saved_state.soldier_is_dead = soldier_is_dead;
    saved_state.soldier_death_frame = soldier_death_frame;
    
    // Save missing boss states
    saved_state.boss_alive = boss_alive;
    saved_state.boss_health = boss_health;
    saved_state.boss_x = boss_x;
    saved_state.boss_y = boss_y;
    saved_state.boss_state = boss_state;
    saved_state.waitingForRunAfterBoss = waitingForRunAfterBoss;
    saved_state.bossMirrored = bossMirrored;
    saved_state.boss_frame_timer = boss_frame_timer;
    
    // Save missing second boss states
    saved_state.second_boss_x = second_boss_x;
    saved_state.second_boss_y = second_boss_y;
    saved_state.second_boss_attacking = second_boss_attacking;
    saved_state.second_boss_attack_timer = second_boss_attack_timer;
    saved_state.second_boss_dead_animation_done = second_boss_dead_animation_done;
    saved_state.second_boss_dead_frame_counter = second_boss_dead_frame_counter;
    saved_state.second_boss_spawn_timer = second_boss_spawn_timer;
    saved_state.second_boss_fire_active = second_boss_fire_active;
    saved_state.second_boss_fire_x = second_boss_fire_x;
    saved_state.second_boss_fire_y = second_boss_fire_y;
    saved_state.second_boss_fire_cooldown = second_boss_fire_cooldown;
    saved_state.second_boss_attack_animating = second_boss_attack_animating;
    saved_state.second_boss_attack_delay = second_boss_attack_delay;
    
    // Save missing boss fire states
    saved_state.boss_fire_active = boss_fire_active;
    saved_state.boss_fire_x = boss_fire_x;
    saved_state.boss_fire_y = boss_fire_y;
    saved_state.boss_fire_left = boss_fire_left;
    
    // Save missing medicine and ammo states
    saved_state.medicine_visible = medicine_visible;
    saved_state.medicine_x = medicine_x;
    saved_state.medicine_y = medicine_y;
    saved_state.medicine_timer = medicine_timer;
    saved_state.ammo_visible = ammo_visible;
    saved_state.ammo_x = ammo_x;
    saved_state.ammo_y = ammo_y;
    
    // Save missing movement states
    saved_state.left = left;
    saved_state.right = right;
    saved_state.bg_flag = bg_flag;
    saved_state.jump = jump;
    saved_state.jump_direction = jump_direction;
    saved_state.mouse_fire_held = mouse_fire_held;
    
    // Save zombie states
    saved_state.total_zombies = total_zombies;
    for (int i = 0; i < MAX_ZOMBIES; i++)
    {
        saved_state.zombie_dead_state[i] = zombie_dead_state[i];
        saved_state.zombie_dead_animation_done[i] = zombie_dead_animation_done[i];
        saved_state.zombie_position_x[i] = zombie_position_x[i];
        saved_state.zombie_position_y[i] = zombie_position_y[i];
        saved_state.zombie_life[i] = zombie_life[i];
        saved_state.zombie_attacking[i] = zombie_attacking[i];
        saved_state.zombie_attack_counter[i] = zombie_attack_counter[i];
        saved_state.zombie_dead_frame_counter[i] = zombie_dead_frame_counter[i];
        saved_state.attack_frame_delay[i] = attack_frame_delay[i];
        saved_state.attack_frame_timer[i] = attack_frame_timer[i];
    }
    
    // Save bullet states
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        saved_state.bullet_fired_r[i] = bullet_fired_r[i];
        saved_state.bullet_fired_l[i] = bullet_fired_l[i];
        saved_state.bullet_position_r_x[i] = bullet_position_r_x[i];
        saved_state.bullet_position_r_y[i] = bullet_position_r_y[i];
        saved_state.bullet_position_l_x[i] = bullet_position_l_x[i];
        saved_state.bullet_position_l_y[i] = bullet_position_l_y[i];
    }

    FILE *f = fopen(SAVE_FILE, "wb");
    if (f)
    {
        fwrite(&saved_state, sizeof(GameState), 1, f);
        fclose(f);
    }
}

void loadGameState()
{
    FILE *f = fopen(SAVE_FILE, "rb");
    if (f)
    {
        fread(&saved_state, sizeof(GameState), 1, f);
        fclose(f);
        if (saved_state.valid)
        {
            can_resume = true;
        }
    }
}

void resetGameData()
{
    // Remove save files
    remove(SAVE_FILE);
    remove(LEADERBOARD_FILE);
    
    // Reset saved state
    saved_state.valid = false;
    can_resume = false;
    
    // Clear leaderboard in memory
    for (int i = 0; i < MAX_LEADERBOARD_SIZE; i++)
    {
        strcpy(leaderboard[i].name, "");
        leaderboard[i].score = 0;
    }
    
    // Reset any other game-related variables to their initial state
    game_Score = 0;
    HIGH_SCORE = 0;
    second_boss_kill_count = 0;
    zombie_wave_count = 0;
}

void resumeGame()
{
    if (!saved_state.valid)
        return;

    is_game_running = true;
    soldier_life = saved_state.soldier_life;
    game_Score = saved_state.game_Score;
    soldier_position_x = saved_state.soldier_position_x;
    soldier_position_y = saved_state.soldier_position_y;
    ammo_count = saved_state.ammo_count;
    boss_phase = saved_state.boss_phase;
    second_boss_kill_count = saved_state.second_boss_kill_count;
    zombie_wave_count = saved_state.zombie_wave_count;
    second_boss_alive = saved_state.second_boss_alive;
    second_boss_spawned = saved_state.second_boss_spawned;
    second_boss_life = saved_state.second_boss_life;
    strcpy(current_player_name, saved_state.player_name);
    
    // Restore missing soldier states
    facing_r = saved_state.facing_r;
    is_jumping = saved_state.is_jumping;
    is_running = saved_state.is_running;
    is_firing = saved_state.is_firing;
    zombie_should_move = saved_state.zombie_should_move;
    soldier_is_dying = saved_state.soldier_is_dying;
    soldier_is_dead = saved_state.soldier_is_dead;
    soldier_death_frame = saved_state.soldier_death_frame;
    
    // Restore missing boss states
    boss_alive = saved_state.boss_alive;
    boss_health = saved_state.boss_health;
    boss_x = saved_state.boss_x;
    boss_y = saved_state.boss_y;
    boss_state = saved_state.boss_state;
    waitingForRunAfterBoss = saved_state.waitingForRunAfterBoss;
    bossMirrored = saved_state.bossMirrored;
    boss_frame_timer = saved_state.boss_frame_timer;
    
    // Restore missing second boss states
    second_boss_x = saved_state.second_boss_x;
    second_boss_y = saved_state.second_boss_y;
    second_boss_attacking = saved_state.second_boss_attacking;
    second_boss_attack_timer = saved_state.second_boss_attack_timer;
    second_boss_dead_animation_done = saved_state.second_boss_dead_animation_done;
    second_boss_dead_frame_counter = saved_state.second_boss_dead_frame_counter;
    second_boss_spawn_timer = saved_state.second_boss_spawn_timer;
    second_boss_fire_active = saved_state.second_boss_fire_active;
    second_boss_fire_x = saved_state.second_boss_fire_x;
    second_boss_fire_y = saved_state.second_boss_fire_y;
    second_boss_fire_cooldown = saved_state.second_boss_fire_cooldown;
    second_boss_attack_animating = saved_state.second_boss_attack_animating;
    second_boss_attack_delay = saved_state.second_boss_attack_delay;
    
    // Restore missing boss fire states
    boss_fire_active = saved_state.boss_fire_active;
    boss_fire_x = saved_state.boss_fire_x;
    boss_fire_y = saved_state.boss_fire_y;
    boss_fire_left = saved_state.boss_fire_left;
    
    // Restore missing medicine and ammo states
    medicine_visible = saved_state.medicine_visible;
    medicine_x = saved_state.medicine_x;
    medicine_y = saved_state.medicine_y;
    medicine_timer = saved_state.medicine_timer;
    ammo_visible = saved_state.ammo_visible;
    ammo_x = saved_state.ammo_x;
    ammo_y = saved_state.ammo_y;
    
    // Restore missing movement states
    left = saved_state.left;
    right = saved_state.right;
    bg_flag = saved_state.bg_flag;
    jump = saved_state.jump;
    jump_direction = saved_state.jump_direction;
    mouse_fire_held = saved_state.mouse_fire_held;
    
    // Restore zombie states AND RE-INITIALIZE SPRITES
    total_zombies = saved_state.total_zombies;
    for (int i = 0; i < MAX_ZOMBIES; i++)
    {
        zombie_dead_state[i] = saved_state.zombie_dead_state[i];
        zombie_dead_animation_done[i] = saved_state.zombie_dead_animation_done[i];
        zombie_position_x[i] = saved_state.zombie_position_x[i];
        zombie_position_y[i] = saved_state.zombie_position_y[i];
        zombie_life[i] = saved_state.zombie_life[i];
        zombie_attacking[i] = saved_state.zombie_attacking[i];
        zombie_attack_counter[i] = saved_state.zombie_attack_counter[i];
        zombie_dead_frame_counter[i] = saved_state.zombie_dead_frame_counter[i];
        attack_frame_delay[i] = saved_state.attack_frame_delay[i];
        attack_frame_timer[i] = saved_state.attack_frame_timer[i];
        
        // RE-INITIALIZE ZOMBIE SPRITES - This was missing!
        if (i < total_zombies)
        {
            // Re-initialize zombie running sprite
            iInitSprite(&zombie_r[i]);
            iChangeSpriteFrames(&zombie_r[i], zombie_run, 7);
            iSetSpritePosition(&zombie_r[i], zombie_position_x[i], zombie_position_y[i]);
            iScaleSprite(&zombie_r[i], 3.75);

            // Re-initialize zombie dead sprite
            iInitSprite(&zombie_d[i]);
            iChangeSpriteFrames(&zombie_d[i], zombie_dead, 5);
            iSetSpritePosition(&zombie_d[i], zombie_position_x[i], zombie_position_y[i]);
            iScaleSprite(&zombie_d[i], 3.75);

            // Re-initialize zombie attack sprite
            iInitSprite(&zombie_a[i]);
            iChangeSpriteFrames(&zombie_a[i], zombie_attack, 5);
            iSetSpritePosition(&zombie_a[i], zombie_position_x[i], zombie_position_y[i]);
            iScaleSprite(&zombie_a[i], 3.75);
        }
    }
    
    // Restore bullet states
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullet_fired_r[i] = saved_state.bullet_fired_r[i];
        bullet_fired_l[i] = saved_state.bullet_fired_l[i];
        bullet_position_r_x[i] = saved_state.bullet_position_r_x[i];
        bullet_position_r_y[i] = saved_state.bullet_position_r_y[i];
        bullet_position_l_x[i] = saved_state.bullet_position_l_x[i];
        bullet_position_l_y[i] = saved_state.bullet_position_l_y[i];
    }

    // Reset sprite positions
    iSetSpritePosition(&soldier_r, soldier_position_x, soldier_position_y);
    iSetSpritePosition(&soldier_i, soldier_position_x, soldier_position_y);
    iSetSpritePosition(&soldier_fr, soldier_position_x, soldier_position_y);
    iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
    
    // Reset boss sprite positions if in boss phase
    if (boss_phase && boss_alive)
    {
        iSetSpritePosition(&boss_i, boss_x, boss_y);
        iSetSpritePosition(&boss_r, boss_x, boss_y);
        iSetSpritePosition(&boss_a, boss_x, boss_y);
        iSetSpritePosition(&boss_ca, boss_x, boss_y);
        iSetSpritePosition(&boss_d, boss_x, boss_y);
        
        // APPLY SAVED MIRROR STATE TO BOSS SPRITES
        // Check if boss sprites need to be mirrored to match saved state
        bool current_mirror_state = boss_i.flipHorizontal; // Check current mirror state
        if (current_mirror_state != bossMirrored)
        {
            // Mirror all boss sprites to match the saved state
            iMirrorSprite(&boss_i, HORIZONTAL);
            iMirrorSprite(&boss_r, HORIZONTAL);
            iMirrorSprite(&boss_a, HORIZONTAL);
            iMirrorSprite(&boss_ca, HORIZONTAL);
            iMirrorSprite(&boss_d, HORIZONTAL);
        }
    }
    
    // Reset second boss sprite positions if needed
    if (second_boss_spawned)
    {
        iSetSpritePosition(&second_boss_spr_idle, second_boss_x, second_boss_y);
        iSetSpritePosition(&second_boss_spr_attack, second_boss_x, second_boss_y);
        iSetSpritePosition(&second_boss_spr_dead, second_boss_x, second_boss_y);
        iSetSpritePosition(&second_boss_spr_walk, second_boss_x, second_boss_y);
        iSetSpritePosition(&second_boss_spr_fire, second_boss_x, second_boss_y);
        iSetSpritePosition(&second_boss_spr_cattack, second_boss_x, second_boss_y);
    }
    
    // Reset boss fire sprite position if active
    if (boss_fire_active)
    {
        iSetSpritePosition(&boss_fire_sprite, boss_fire_x, boss_fire_y);
    }
}

void cleanup()
{
    // Only save game state if the game is actively running and not over
    if (is_game_running && !is_game_over && !is_victory && !soldier_is_dead)
    {
        saveGameState();
    }
}

void loadLeaderboard()
{
    FILE *f = fopen(LEADERBOARD_FILE, "r");
    if (f)
    {
        for (int i = 0; i < MAX_LEADERBOARD_SIZE; i++)
        {
            fscanf(f, "%s %d", leaderboard[i].name, &leaderboard[i].score);
        }
        fclose(f);
    }
    else
    {
        // initialize empty leaderboard
        for (int i = 0; i < MAX_LEADERBOARD_SIZE; i++)
        {
            strcpy(leaderboard[i].name, "---");
            leaderboard[i].score = 0;
        }
        saveLeaderboard();
    }
}

void updateLeaderboard(const char *name, int score)
{
    // if player already exists
    int existingPos = -1;
    for (int i = 0; i < MAX_LEADERBOARD_SIZE; i++)
    {
        if (strcmp(leaderboard[i].name, name) == 0)
        {
            existingPos = i;
            break;
        }
    }

    // player exists and new score is better, remove old entry
    if (existingPos != -1)
    {
        if (score <= leaderboard[existingPos].score)
        {
            return;
        }
        // remove existing entry
        for (int i = existingPos; i < MAX_LEADERBOARD_SIZE - 1; i++)
        {
            strcpy(leaderboard[i].name, leaderboard[i + 1].name);
            leaderboard[i].score = leaderboard[i + 1].score;
        }
        strcpy(leaderboard[MAX_LEADERBOARD_SIZE - 1].name, "---");
        leaderboard[MAX_LEADERBOARD_SIZE - 1].score = 0;
    }

    int insertPos = -1;
    for (int i = 0; i < MAX_LEADERBOARD_SIZE; i++)
    {
        if (strcmp(leaderboard[i].name, "---") == 0 || score > leaderboard[i].score)
        {
            insertPos = i;
            break;
        }
    }

    if (insertPos == -1)
    {
        // Score is too low to make it to top 5
        return;
    }

    // Shift scores down to make room for new score
    for (int i = MAX_LEADERBOARD_SIZE - 1; i > insertPos; i--)
    {
        strcpy(leaderboard[i].name, leaderboard[i - 1].name);
        leaderboard[i].score = leaderboard[i - 1].score;
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
    if (!is_game_running || is_game_over || paused)
        return;
    medicine_timer += 150;
    if (!medicine_visible && medicine_timer >= medicine_respawn_time && !boss_phase)
    {
        spawnMedicine();
        medicine_timer = 0;
    }
}
void updateJump()
{
    if (!is_jumping)
        return;
    soldier_position_y += jump_speed * jump_direction;
    soldier_i.y += jump_speed * jump_direction;
    soldier_r.y += jump_speed * jump_direction;
    soldier_fr.y += jump_speed * jump_direction;
    soldier_d.y += jump_speed * jump_direction;

    if (soldier_position_y >= ground_y + jump_height)
    {
        jump_direction = -1;
    }
    else if (soldier_position_y <= ground_y)
    {
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

    if (is_game_over)
    {
        if (game_Score > 0)
        {
            updateLeaderboard(current_player_name, game_Score);
        }
        // clear saved game when game is over
        can_resume = false;
        saved_state.valid = false;
        remove(SAVE_FILE);

        if (is_victory)
        {
            iShowLoadedImage(0, 0, &victory);
        }
        else
        {
            iShowLoadedImage(50, 50, &gameover);
        }
        iSetColor(255, 255, 0);
        iText(500, 80, "Press 'h' to return main menu", GLUT_BITMAP_HELVETICA_18);
        iSetColor(255, 255, 255);
        return;
    }

    if (!is_game_running)
    {
        iShowLoadedImage(0, 0, &menu_background);
        if (is_entering_name)
        {
            iSetColor(255, 255, 0);
            iText(550, 400, "Enter your name:", GLUT_BITMAP_HELVETICA_18);
            iSetColor(255, 255, 255);
            iText(550, 350, current_player_name, GLUT_BITMAP_HELVETICA_18);
            iText(550, 300, "Press ENTER to start", GLUT_BITMAP_HELVETICA_12);
            return;
        }
        if (show_high_score_screen)
        {
            iSetColor(255, 255, 0);
            iText(600, 500, "LEADERBOARD", GLUT_BITMAP_HELVETICA_18);
            for (int i = 0; i < MAX_LEADERBOARD_SIZE; i++)
            {
                char scoreText[64];
                sprintf(scoreText, "%d. %s: %d", i + 1, leaderboard[i].name, leaderboard[i].score);
                iText(600, 450 - i * 40, scoreText, GLUT_BITMAP_HELVETICA_18);
            }
            iSetColor(255, 255, 255);
            iText(600, 200, "Press ESC to return", GLUT_BITMAP_HELVETICA_12);
            return;
        }
        if (show_settings)
        {
            iSetColor(255, 255, 0);
            iText(600, 500, "SETTINGS", GLUT_BITMAP_HELVETICA_18);

            // Zombie sound setting
            iSetColor(255, 255, 255);
            iText(500, 400, "Zombie Sound:", GLUT_BITMAP_HELVETICA_18);
            if (zombie_sound_enabled)
            {
                iSetColor(0, 255, 0);
                iText(700, 400, "ON", GLUT_BITMAP_HELVETICA_18);
            }
            else
            {
                iSetColor(255, 0, 0);
                iText(700, 400, "OFF", GLUT_BITMAP_HELVETICA_18);
            }

            // Bullet sound setting
            iSetColor(255, 255, 255);
            iText(500, 350, "Bullet Sound:", GLUT_BITMAP_HELVETICA_18);
            if (bullet_sound_enabled)
            {
                iSetColor(0, 255, 0);
                iText(700, 350, "ON", GLUT_BITMAP_HELVETICA_18);
            }
            else
            {
                iSetColor(255, 0, 0);
                iText(700, 350, "OFF", GLUT_BITMAP_HELVETICA_18);
            }

            iSetColor(255, 255, 255);
            iText(550, 280, "Press '1' to toggle Zombie Sound", GLUT_BITMAP_HELVETICA_12);
            iText(550, 250, "Press '2' to toggle Bullet Sound", GLUT_BITMAP_HELVETICA_12);
            iText(500, 220, "Press Delete Button for resetting the game", GLUT_BITMAP_HELVETICA_12);
            iText(600, 200, "Press ESC to return", GLUT_BITMAP_HELVETICA_12);
            return;
        }
        if (show_credits)
        {
            iShowLoadedImage(0, 0, &credits);
            return;
        }
        if (show_help)
        {
            iShowLoadedImage(0, 0, &help);
            return;
        }
        int visibleButtons = 0;
        for (int i = 0; i < BUTTON_COUNT; i++)
        {
            if (i == 1 && !can_resume)
                continue;
            visibleButtons++;
        }

        int adjustedMenuYStart = menu_y_start + ((BUTTON_COUNT - visibleButtons) * (menu_height + menu_spacing)) / 2;

        int current_visible_index = 0;
        for (int i = 0; i < BUTTON_COUNT; i++)
        {

            if (i == 1 && !can_resume)
                continue;

            int y = adjustedMenuYStart - current_visible_index * (menu_height + menu_spacing);

            if (i == hovered_index)
                iSetColor(20, 250, 235);
            else if (i == 1 && !can_resume)
                iSetColor(100, 100, 100);
            else
                iSetColor(255, 255, 255);

            iText(menu_x, y, menu_items[i], GLUT_BITMAP_HELVETICA_18);
            current_visible_index++;
        }
    }
    else
    {

        iShowLoadedImage(0, 0, &bg);
        if (paused)
        {
            iSetColor(255, 255, 0);
            iText(600, 300, "PAUSED", GLUT_BITMAP_HELVETICA_18);
            iSetColor(255, 255, 255);
            iText(570, 260, "Press 'r' to resume", GLUT_BITMAP_HELVETICA_12);
            iText(570, 230, "Press 'HOME' to go to menu", GLUT_BITMAP_HELVETICA_12);
            return;
        }

        if (is_game_running && !is_game_over)
        {

            int maxLife = 200;
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
            if (!soldier_is_dead && soldier_life > 0)
            {
                iSetColor(0, 200, 0);
                iFilledRectangle(x, y, (int)(barWidth * ((float)soldier_life / maxLife)), barHeight);
            }
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
            // gameScore += 1;

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
            else if (second_boss_x < soldier_position_x + 10)
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
            iFilledRectangle(900, 560, 200 * boss_health / boss_health_max, 25);
            iSetColor(255, 255, 255);
            iRectangle(900, 560, 200, 25);
            iSetColor(0, 0, 0);
            iText(910, 573, "Boss HP", GLUT_BITMAP_HELVETICA_12);
            iSetColor(255, 255, 255);
        }
        // display the score in yellow
        char scoreText[32];
        sprintf(scoreText, "SCORE : %d", game_Score);
        iSetColor(255, 255, 0);
        iText(600, 570, scoreText, GLUT_BITMAP_HELVETICA_18);
        if (waitingForRunAfterBoss)
        {
            iSetColor(255, 0, 0);
            iText(1100, 300, "GO GO -->", GLUT_BITMAP_HELVETICA_18);
        }

        // draw ammo count at top left
        char ammoText[32];
        sprintf(ammoText, "AMMO : %d", ammo_count);
        iSetColor(0, 200, 255);
        iText(20, 530, ammoText, GLUT_BITMAP_HELVETICA_18);
        // draw ammo box if visible
        if (ammo_visible)
        {
            iShowLoadedImage(ammo_x, ammo_y, &ammo);
        }
    }
}

void iMouseMove(int mx, int my)
{
    if (!is_game_running && !is_entering_name && !show_high_score_screen && !show_credits && !show_help && !show_settings)
    {
        hovered_index = -1;

        int visibleButtons = 0;
        for (int i = 0; i < BUTTON_COUNT; i++)
        {
            if (i == 1 && !can_resume)
                continue;
            visibleButtons++;
        }

        int adjustedMenuYStart = menu_y_start + ((BUTTON_COUNT - visibleButtons) * (menu_height + menu_spacing)) / 2;

        int current_visible_index = 0;

        for (int i = 0; i < BUTTON_COUNT; i++)
        {
            // skip the RESUME button if we can't resume
            if (i == 1 && !can_resume)
                continue;

            int y = adjustedMenuYStart - current_visible_index * (menu_height + menu_spacing);

            // check if mouse is over this button
            if (mx >= menu_x - 10 && mx <= menu_x + menu_width + 10 &&
                my >= y - menu_height / 2 && my <= y + menu_height / 2)
            {
                hovered_index = i;
                break;
            }

            current_visible_index++;
        }
    }
}

void iMouse(int button, int state, int mx, int my)
{
    if (!is_game_running && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && hovered_index != -1)
    {
        iPlaySound("assets/sounds/clicked.wav", false);
        if (hovered_index == 0) // PLAY
        {
            is_entering_name = true;
            current_player_name[0] = '\0';
        }
        else if (hovered_index == 1) // RESUME
        {
            if (can_resume)
            {
                resumeGame();
            }
        }
        else if (hovered_index == 2) // LEADERBOARD
        {
            show_high_score_screen = true;
        }
        else if (hovered_index == 3) // SETTINGS
        {
            show_settings = true;
        }
        else if (hovered_index == 4) // HELP
        {
            show_help = true;
        }
        else if (hovered_index == 5) // CREDITS
        {
            show_credits = true;
        }
        else if (hovered_index == 6) // EXIT
        {
            exit(0);
        }
    }

    if (is_game_running)
    {
        if (button == GLUT_LEFT_BUTTON && (state == GLUT_DOWN || state == GLUT_HOLD))
        {
            if (is_jumping)
                return;
            mouse_fire_held = true;
            is_firing = true;
            is_running = false;
            if (ammo_count > 0)
            {
                ammo_count--;
                if (left)
                {
                    if (!soldier_fr.flipHorizontal)
                    {
                        iMirrorSprite(&soldier_fr, HORIZONTAL);
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
                    if (soldier_fr.flipHorizontal)
                    {
                        iMirrorSprite(&soldier_fr, HORIZONTAL);
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
                else
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
                zombie_should_move = true;
                if (ammo_count == 0 && !ammo_visible)
                {
                    spawnAmmoBox();
                }
            }
        }
        if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        {
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
    if (is_entering_name)
    {
        if (state == GLUT_DOWN)
        {
            if (key == '\r')
            {
                if (strlen(current_player_name) > 0)
                {
                    is_entering_name = false;
                    resetGameState();
                }
                return;
            }
            else if (key == 8)
            {
                int len = strlen(current_player_name);
                if (len > 0)
                    current_player_name[len - 1] = '\0';
            }
            else if (strlen(current_player_name) < MAX_NAME_LENGTH - 1)
            {
                if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') ||
                    (key >= '0' && key <= '9') || key == ' ')
                {
                    int len = strlen(current_player_name);
                    current_player_name[len] = key;
                    current_player_name[len + 1] = '\0';
                }
            }
            return;
        }
        return;
    }

    if (key == 'p' && state == GLUT_DOWN)
    {
        paused = true;
        return;
    }
    if (key == 'r' && state == GLUT_DOWN)
    {
        paused = false;
        return;
    }

    if ((is_game_over || is_victory) && key == 'h' && state == GLUT_DOWN)
    {
        is_game_running = false;
        is_game_over = false;
        is_victory = false;
        show_high_score_screen = false;
        show_credits = false;
        show_help = false;
        return;
    }

    if (!is_game_running)
    {
        if (show_settings)
        {
            if (key == 27)
            { // ESC key to exit settings
                show_settings = false;
            }
            else if (key == '1' && state == GLUT_DOWN)
            { // Toggle zombie sound
                zombie_sound_enabled = !zombie_sound_enabled;
                if (!zombie_sound_enabled && zombie_sound_channel != -1)
                {
                    iStopSound(zombie_sound_channel);
                    zombie_sound_channel = -1;
                }
            }
            else if (key == '2' && state == GLUT_DOWN)
            { // Toggle bullet sound
                bullet_sound_enabled = !bullet_sound_enabled;
            }
            else if (key == 127 && state == GLUT_DOWN)
            { // Delete key to reset game data
                resetGameData();
                // You could add a confirmation message here if needed
            }
            return;
        }

        if (show_high_score_screen && key == 27)
        {
            show_high_score_screen = false;
        }
        if (show_credits && key == 27)
        {
            show_credits = false;
        }
        if (show_help && key == 27)
        {
            show_help = false;
        }
        return;
    }

    if (state == GLUT_UP)
    {
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

    if (key == 'd' && state == GLUT_DOWN && !is_firing) // Only allow movement if not firing
    {
        is_running = true;
        is_firing = false;
        right = true;
        left = false;
        bg_flag = 1;
        zombie_should_move = true;

        if (soldier_r.flipHorizontal)
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL);
        }
        facing_r = true; // Always set facing right when moving right

        // No special handling for waitingForRunAfterBoss
    }
    if (key == 'a' && state == GLUT_DOWN && !is_firing) // Only allow movement if not firing
    {
        is_firing = false;
        is_running = true;
        left = true;
        right = false;
        bg_flag = 2;

        if (!soldier_r.flipHorizontal)
        {
            iMirrorSprite(&soldier_r, HORIZONTAL);
            iMirrorSprite(&soldier_fr, HORIZONTAL);
            iMirrorSprite(&soldier_i, HORIZONTAL);
        }
        facing_r = false; // Always set facing left when moving left
    }
    if (key == 'w' && !is_jumping)
    {
        is_jumping = true;
        is_running = false;
        is_firing = false;
    }
    if (!left && !right && is_firing)
    {
        // When firing without movement, use current facing direction
        // Only mirror if the fire sprite doesn't match current facing direction
        if (!facing_r && !soldier_fr.flipHorizontal)
        {
            iMirrorSprite(&soldier_fr, HORIZONTAL);
        }
        else if (facing_r && soldier_fr.flipHorizontal)
        {
            iMirrorSprite(&soldier_fr, HORIZONTAL);
        }

        if (facing_r)
        {
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
        else
        {
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
    }
}

void iSpecialKeyboard(int key, int state)
{
    if (!is_game_running)
    {
        return;
    }

    if (key == GLUT_KEY_END)
    {
        exit(0);
    }
    if (key == GLUT_KEY_HOME)
    {
        if (!is_game_over && is_game_running)
        { // Only save if game is active
            saveGameState();
            can_resume = true;
        }
        is_game_running = false;
        show_high_score_screen = false;
        show_credits = false;
        show_help = false;
        medicine_timer = 0;
        medicine_x = 0;
        medicine_y = 128;
        second_boss_spawned = false;
        second_boss_alive = false;
        second_boss_kill_count = 0;
        zombie_wave_count = 0;
        boss_phase = false;
        boss_alive = false;
        return;
    }
    if (!is_game_running)
    {
        return;
    }

    if (key == GLUT_KEY_END)
    {
        exit(0);
    }

    if (state == GLUT_UP)
    {
        is_running = false;
        bg_flag = 0;
        left = false;
        right = false;
        iSetSpritePosition(&soldier_i, soldier_position_x, soldier_position_y);
        return;
    }

    if (key == GLUT_KEY_LEFT && state == GLUT_DOWN && !is_firing) // Only allow movement if not firing
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

    if (key == GLUT_KEY_RIGHT && state == GLUT_DOWN && !is_firing) // Only allow movement if not firing
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
        // No special handling for waitingForRunAfterBoss
    }
    if (key == GLUT_KEY_DOWN && state == GLUT_DOWN)
    {
        iKeyboard('s', GLUT_DOWN);
    }
    if (key == GLUT_KEY_UP && state == GLUT_DOWN)
    {
        iKeyboard('w', GLUT_DOWN);
    }
}

void boss_update()
{
    if (!boss_phase || !boss_alive || is_game_over)
        return;
    bool should_mirror = (soldier_position_x < boss_x);
    if (should_mirror != bossMirrored)
    {
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

    if (abs(boss_x - soldier_position_x) < boss_cattack_range)
    {
        boss_state = 4;
        boss_cattack_frame++;
        boss_attack_anim_frame = 0;
        boss_attack_anim_playing = false;
        fireball_queued = false;
        if (boss_cattack_frame % 14 == 1 && !soldier_is_dead)
        {
            if (soldier_life > 0)
                soldier_life -= 5;
            if (soldier_life <= 0)
            {
                soldier_is_dead = true;
                iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
                can_resume = false;
                saved_state.valid = false;
                remove(SAVE_FILE);
            }
        }
    }
    else
    {
        boss_cattack_frame = 0;
        if (!boss_attack_anim_playing && !boss_fire_active)
        {
            boss_attack_anim_playing = true;
            boss_attack_anim_frame = 0;
            fireball_queued = false;
        }
        if (boss_attack_anim_playing)
        {
            boss_state = 2;
            boss_attack_anim_frame += 2;
            if (boss_attack_anim_frame >= 8)
            {
                boss_attack_anim_playing = false;
                boss_attack_anim_frame = 0;
                fireball_queued = true;
            }
        }
        else
        {
            boss_state = 1;
            if (soldier_position_x > boss_x + 50)
            {
                boss_x += boss_speed;
            }
            else if (soldier_position_x < boss_x)
            {
                boss_x -= boss_speed;
            }
        }

        if (fireball_queued && !boss_fire_active)
        {
            boss_fire_active = true;
            boss_fire_x = boss_x + (bossMirrored ? -30 : 130);
            boss_fire_y = boss_y + 80;
            boss_fire_left = (soldier_position_x < boss_x);
            if (boss_fire_left && !boss_fire_mirrored)
            {
                iMirrorSprite(&boss_fire_sprite, HORIZONTAL);
                boss_fire_mirrored = true;
            }
            else if (!boss_fire_left && boss_fire_mirrored)
            {
                iMirrorSprite(&boss_fire_sprite, HORIZONTAL);
                boss_fire_mirrored = false;
            }
            fireball_queued = false;
        }
    }

    if (boss_fire_active)
    {
        int fire_speed = 40;
        if (boss_fire_left)
            boss_fire_x -= fire_speed;
        else
            boss_fire_x += fire_speed;
        iSetSpritePosition(&boss_fire_sprite, boss_fire_x, boss_fire_y);
        if (checkCollision(boss_fire_x, boss_fire_y, 40, 40, soldier_position_x, soldier_position_y, 100, 100))
        {
            boss_fire_active = false;
            boss_fire_x = -1000;
            if (soldier_life > 0)
                soldier_life -= 7;
            if (soldier_life <= 0)
            {
                soldier_is_dead = true;
                iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
                can_resume = false;
                saved_state.valid = false;
                remove(SAVE_FILE);
            }
        }
        else if (boss_fire_x < -100 || boss_fire_x > 1500)
        {
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
    if (!is_game_running || paused)
        return;
    updateJump();
    if (!soldier_is_dead)
    {
        iAnimateSprite(&soldier_i);
        if (is_running && !is_jumping && !is_firing) // Added check to prevent movement when firing
        {
            const int screen_middle = 430;
            const int transition_target = 450; // Target position for waitingForRunAfterBoss transition
            const int right_boundary = 1050;
            const int left_boundary = 0;

            // Special handling for waitingForRunAfterBoss state - disabled as requested
            if (waitingForRunAfterBoss)
            {
                // Just clear the flag immediately without any special movement
                waitingForRunAfterBoss = false;
            }

            if (!(boss_phase && boss_alive) && !second_boss_alive)
            {
                if (right)
                {
                    bool blocked = false;
                    for (int i = 0; i < total_zombies; i++)
                    {
                        if (checkCollision(zombie_position_x[i], zombie_position_y[i], 100, 100,
                                           soldier_position_x, soldier_position_y, 100, 100))
                        {
                            blocked = true;
                            break;
                        }
                    }
                    if (!blocked)
                    {
                        if (soldier_position_x < screen_middle)
                        {
                            int move_dist = 35;
                            // don't move past screen middle
                            if (soldier_position_x + move_dist > screen_middle)
                            {
                                move_dist = screen_middle - soldier_position_x;
                            }
                            // don't move past right boundary
                            if (soldier_position_x + move_dist > right_boundary)
                            {
                                move_dist = right_boundary - soldier_position_x;
                            }
                            if (move_dist > 0)
                            {
                                soldier_position_x += move_dist;
                                soldier_r.x += move_dist;
                                soldier_i.x += move_dist;
                                soldier_fr.x += move_dist;
                            }
                        }
                        else if (soldier_position_x == screen_middle)
                        {
                            iWrapImage(&bg, -22);
                            if (medicine_visible)
                            {
                                medicine_x -= 22;
                            }
                            if (ammo_visible)
                            {
                                ammo_x -= 22;
                            }
                        }
                        if (soldier_position_x > screen_middle)
                        {
                            int diff = soldier_position_x - screen_middle;
                            soldier_position_x = screen_middle;
                            soldier_r.x -= diff;
                            soldier_i.x -= diff;
                            soldier_fr.x -= diff;
                        }
                        zombie_should_move = true;
                        // gameScore += 1;
                    }
                }
                if (left && soldier_position_x > 0)
                {
                    int move_dist = 35;
                    if (soldier_position_x - move_dist < 0)
                        move_dist = soldier_position_x;
                    soldier_position_x -= move_dist;
                    soldier_r.x -= move_dist;
                    soldier_i.x -= move_dist;
                    soldier_fr.x -= move_dist;
                    // gameScore += 1;
                }
            }
            else
            {
                if (right)
                {
                    bool blocked = false;
                    for (int i = 0; i < total_zombies; i++)
                    {
                        if (checkCollision(zombie_position_x[i], zombie_position_y[i], 100, 100,
                                           soldier_position_x, soldier_position_y, 100, 100))
                        {
                            blocked = true;
                            break;
                        }
                    }
                    if (!blocked && soldier_position_x < right_boundary)
                    {
                        int move_dist = 30;
                        if (soldier_position_x + move_dist > right_boundary)
                        {
                            move_dist = right_boundary - soldier_position_x;
                        }
                        if (move_dist > 0)
                        {
                            soldier_position_x += move_dist;
                            soldier_r.x += move_dist;
                            soldier_i.x += move_dist;
                            soldier_fr.x += move_dist;
                            zombie_should_move = true;
                            // gameScore += 1;
                        }
                    }
                }
                if (left && soldier_position_x > 0)
                { // Ensure soldier doesn't go past left boundary
                    int move_dist = 22;
                    if (soldier_position_x - move_dist < 0)
                    {
                        move_dist = soldier_position_x; // Adjust movement to stop exactly at boundary
                    }
                    soldier_position_x -= move_dist;
                    soldier_r.x -= move_dist;
                    soldier_i.x -= move_dist;
                    soldier_fr.x -= move_dist;
                    // gameScore += 1;
                }
            }
            // Remove duplicate left movement code to prevent double movement

            iAnimateSprite(&soldier_r);
        }
        static int mouse_fire_delay = 0;
        if (mouse_fire_held && !is_jumping && ammo_count > 0)
        {
            is_running = false; // Stop running when firing
            mouse_fire_delay++;
            if (mouse_fire_delay >= 1)
            {
                ammo_count--;
                // Use current movement direction or last facing direction
                bool shoot_right = facing_r;
                if (left)
                {
                    shoot_right = false;
                    if (facing_r)
                    {
                        iMirrorSprite(&soldier_fr, HORIZONTAL);
                        facing_r = false;
                    }
                }
                else if (right)
                {
                    shoot_right = true;
                    if (!facing_r)
                    {
                        iMirrorSprite(&soldier_fr, HORIZONTAL);
                        facing_r = true;
                    }
                }

                if (shoot_right)
                {
                    for (int i = 0; i < MAX_BULLETS; i++)
                    {
                        if (!bullet_fired_r[i])
                        {
                            bullet_fired_r[i] = true;
                            bullet_position_r_x[i] = soldier_position_x + 150;
                            bullet_position_r_y[i] = 250;
                            if (bullet_sound_enabled)
                            {
                                iPlaySound("assets/sounds/shoot.wav", false);
                            }
                            break;
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < MAX_BULLETS; i++)
                    {
                        if (!bullet_fired_l[i])
                        {
                            bullet_fired_l[i] = true;
                            bullet_position_l_x[i] = soldier_position_x - 40;
                            bullet_position_l_y[i] = 250;
                            if (bullet_sound_enabled)
                            {
                                iPlaySound("assets/sounds/shoot.wav", false);
                            }
                            break;
                        }
                    }
                }
                zombie_should_move = true;
                if (ammo_count == 0 && !ammo_visible)
                {
                    spawnAmmoBox();
                }
                mouse_fire_delay = 0;
            }
        }
        else
        {
            mouse_fire_delay = 2;
        }
        static int f_key_fire_delay = 0;
        if (is_firing && !is_jumping && ammo_count > 0)
        {
            is_running = false; // Stop running when firing
            f_key_fire_delay++;
            if (f_key_fire_delay >= 1)
            {
                ammo_count--;
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
                            if (bullet_sound_enabled)
                            {
                                iPlaySound("assets/sounds/shoot.wav", false);
                            }
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
                            if (bullet_sound_enabled)
                            {
                                iPlaySound("assets/sounds/shoot.wav", false);
                            }
                            break;
                        }
                    }
                }
                else
                {
                    // When no movement keys are pressed, use current facing direction
                    if (facing_r)
                    {
                        if (soldier_fr.flipHorizontal)
                        {
                            iMirrorSprite(&soldier_fr, HORIZONTAL);
                        }
                        for (int i = 0; i < MAX_BULLETS; i++)
                        {
                            if (!bullet_fired_r[i])
                            {
                                bullet_fired_r[i] = true;
                                bullet_position_r_x[i] = soldier_position_x + 150;
                                bullet_position_r_y[i] = 250;
                                if (bullet_sound_enabled)
                                {
                                    iPlaySound("assets/sounds/shoot.wav", false);
                                }
                                break;
                            }
                        }
                    }
                    else
                    {
                        if (!soldier_fr.flipHorizontal)
                        {
                            iMirrorSprite(&soldier_fr, HORIZONTAL);
                        }
                        for (int i = 0; i < MAX_BULLETS; i++)
                        {
                            if (!bullet_fired_l[i])
                            {
                                bullet_fired_l[i] = true;
                                bullet_position_l_x[i] = soldier_position_x - 40;
                                bullet_position_l_y[i] = 250;
                                if (bullet_sound_enabled)
                                {
                                    iPlaySound("assets/sounds/shoot.wav", false);
                                }
                                break;
                            }
                        }
                    }
                }
                zombie_should_move = true;
                if (ammo_count == 0 && !ammo_visible)
                {
                    spawnAmmoBox();
                }
                f_key_fire_delay = 0;
            }
        }
        else
        {
            f_key_fire_delay = 2;
        }
        if (!is_firing && !is_jumping)
        {
            iSetSpritePosition(&soldier_i, soldier_position_x, soldier_position_y);
        }
        if (is_firing && !is_jumping)
            iAnimateSprite(&soldier_fr);
    }
    else
    {
        iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
        if (soldier_death_frame < 3)
        {
            iAnimateSprite(&soldier_d);
            soldier_death_frame++;
        }
        else
        {
            game_over_timer++;
            if (game_over_timer >= 10)
            {
                is_game_over = true;
                is_game_running = false;
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
                    game_Score += 10;
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
                                iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
                                can_resume = false;
                                saved_state.valid = false;
                                remove(SAVE_FILE);
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
                    zombie_position_x[i] = 1200 + rand() % 400 + i * 150;
                    iSetSpritePosition(&zombie_r[i], zombie_position_x[i], zombie_position_y[i]);
                    iSetSpritePosition(&zombie_d[i], zombie_position_x[i], zombie_position_y[i]);
                    iSetSpritePosition(&zombie_a[i], zombie_position_x[i], zombie_position_y[i]);
                }
            }
        }

        if (medicine_visible && checkCollision(soldier_position_x, soldier_position_y, 100, 100, medicine_x, medicine_y, 30, 30))
        {
            if (soldier_life < maxLife)
                soldier_life+=15;
            medicine_visible = false;
            medicine_timer = 0;
        }

        if (second_boss_alive && !second_boss_dead_animation_done)
        {
            if (second_boss_x < soldier_position_x)
            {
                second_boss_x = soldier_position_x;
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
                    {
                        soldier_is_dead = true;
                        iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
                        can_resume = false;
                        saved_state.valid = false;
                        remove(SAVE_FILE);
                    }
                }
            }
            else
            {
                second_boss_attack_timer = 0;
            }
            if (!second_boss_fire_active && !second_boss_attack_animating)
            {
                if (second_boss_x >= 0 && second_boss_x <= 1100)
                {
                    second_boss_fire_cooldown++;
                    if (second_boss_fire_cooldown >= second_boss_fire_cooldown_max)
                    {
                        second_boss_attack_animating = true;
                        second_boss_spr_attack.currentFrame = 0;
                        second_boss_fire_cooldown = 0;
                    }
                }
            }

            if (second_boss_attack_animating)
            {
                if (second_boss_x >= 0 && second_boss_x <= 1100)
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
                else
                {
                    second_boss_attack_animating = false;
                }
            }
            if (second_boss_fire_active)
            {
                if (second_boss_x >= -100 && second_boss_x <= 1100)
                {
                    second_boss_fire_x -= second_boss_fire_speed;

                    if (abs(second_boss_x - soldier_position_x) <= 200 || checkCollision(second_boss_fire_x, second_boss_fire_y, 30, 30, soldier_position_x, soldier_position_y, 30, 100))
                    {
                        second_boss_fire_active = false;
                        if (soldier_life > 0)
                            soldier_life -= 9;
                        if (soldier_life <= 0)
                        {
                            soldier_is_dead = true;
                            iSetSpritePosition(&soldier_d, soldier_position_x, soldier_position_y);
                            can_resume = false;
                            saved_state.valid = false;
                            remove(SAVE_FILE);
                        }
                    }
                    else if (second_boss_fire_x < -100)
                    {
                        second_boss_fire_active = false;
                    }
                }
                else
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
                if (second_boss_alive)
                {
                    game_Score += 500;
                    second_boss_alive = false;
                    second_boss_attack_animating = false;
                    second_boss_fire_active = false;
                    second_boss_dead_frame_counter = 0;
                    second_boss_kill_count++;
                    waitingForRunAfterBoss = true;
                    if (second_boss_kill_count >= 5 && !boss_phase)
                    {
                        boss_phase = true;
                        boss_alive = true;
                        boss_health = boss_health_max;
                        medicine_visible = false;
                        for (int i = 0; i < total_zombies; i++)
                        {
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
                        boss_x = 950;
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
                zombie_wave_count = 0; // Reset for next cycle

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
    if (!is_game_running || paused)
        return;
    if (boss_phase && boss_alive)
    {
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullet_fired_r[i])
            {
                bullet_position_r_x[i] += 25;
                if (bullet_position_r_x[i] > 1200)
                {
                    bullet_fired_r[i] = false;
                    bullet_position_r_x[i] = -100;
                }
                if (boss_alive && bullet_position_r_x[i] > boss_x + 170 && soldier_position_x < boss_x + 150)
                {
                    bullet_fired_r[i] = false;
                    bullet_position_r_x[i] = -100;
                    boss_health--;
                    if (boss_health <= 0)
                    {
                        game_Score += 1000;
                        boss_alive = false;
                        is_victory = true;
                        is_game_over = true;
                        is_game_running = false;
                        can_resume = false;
                        saved_state.valid = false;
                        remove(SAVE_FILE);
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
                if (boss_alive && bullet_position_l_x[i] < boss_x + 180 && soldier_position_x > boss_x)
                {
                    bullet_fired_l[i] = false;
                    bullet_position_l_x[i] = -100;
                    boss_health--;
                    if (boss_health <= 0)
                    {
                        game_Score += 1000;
                        boss_alive = false;
                        is_victory = true;
                        is_game_over = true;
                        is_game_running = false;
                        can_resume = false;
                        saved_state.valid = false;
                        remove(SAVE_FILE);
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
            if (bullet_position_r_x[i] > 1200)
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
                        game_Score += 10;
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
                        game_Score += 10;
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
                        game_Score += 10;
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
                        game_Score += 10;
                        zombie_dead_frame_counter[j] = 0;
                        iSetSpritePosition(&zombie_d[j], zombie_position_x[j], zombie_position_y[j]);
                    }
                }
            }
        }
        if (second_boss_alive && !second_boss_dead_animation_done)
        {
            if (bullet_fired_r[i] && bullet_position_r_x[i] > second_boss_x + 150 && abs(bullet_position_r_x[i] - second_boss_x) < 250)
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
            if (second_boss_life <= 0 && second_boss_alive)
            {
                game_Score += 500;
                second_boss_alive = false;
                second_boss_attack_animating = false;
                second_boss_fire_active = false;
                second_boss_dead_frame_counter = 0;
                second_boss_kill_count++;
                waitingForRunAfterBoss = true;

                if (second_boss_kill_count >= 5 && !boss_phase)
                {
                    boss_phase = true;
                    boss_alive = true;
                    boss_health = boss_health_max;
                    medicine_visible = false;

                    for (int z = 0; z < total_zombies; z++)
                    {
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

                    boss_x = 950;
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

void checkAmmoPickup()
{
    if (!is_game_running || paused || is_game_over)
        return;

    if (ammo_visible && checkCollision(soldier_position_x, soldier_position_y, 100, 100, ammo_x, ammo_y, 50, 50))
    {
        ammo_count = 100;
        ammo_visible = false;
    }
}

void update_boss_state()
{
    if (!second_boss_alive || second_boss_life <= 0)
    {

        game_Score += 500;
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
    iOpenWindow(1200, 600, "Zombie Shooter");
    return 0;
}