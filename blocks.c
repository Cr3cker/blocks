#include <raylib.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define PLAYER_WIDTH 70.0
#define PLAYER_HEIGHT 10.0
#define BRICK_WIDTH 80
#define BRICK_HEIGHT 20
#define VOLUME 150.0


time_t game_over_time;


typedef struct {
    Vector2 pos;
    bool is_game_over;
} Player;


typedef struct {
    Vector2 pos;
    float radius;
    bool active;
    Vector2 speed;
} Ball;


typedef struct {
    Vector2 pos;
    bool is_visible;
} Brick;

Brick matrix[10][10];

void start_game(Sound start_sound) {
    int val[3] = {1, 2, 3};

    PlaySound(start_sound);

    for (int i = 0; i < 3; i++) {
        BeginDrawing();
        ClearBackground(BLACK);

        const char *text = TextFormat("%d", val[i]);
        int textWidth = MeasureText(text, 200);
        int textHeight = 100 * 1.5; 

        DrawText(text, WINDOW_WIDTH / 2 - textWidth / 2, WINDOW_HEIGHT / 2 - textHeight / 2, 200, WHITE);

        EndDrawing();
        WaitTime(1); 
    }
}

void init_game(Player *player, Ball *ball, Sound start_sound) {
    start_game(start_sound);

    float x = 0; 
    float y = 0;

    for (int i = 0; i < 10; i++) {
        Brick brick = { 0 };
        for (int j = 0; j < 10; j++) {
            brick.pos.x = x;
            brick.pos.y = y;
            brick.is_visible = true;
            matrix[i][j] = brick;

            x += BRICK_WIDTH + 1; 
        }
        y += BRICK_HEIGHT + 1; 
        x = 0;
    }

    player->pos.x = WINDOW_WIDTH / 2 - PLAYER_WIDTH / 2;
    player->pos.y = WINDOW_HEIGHT - 200 - PLAYER_HEIGHT / 2;
    player->is_game_over = false;

    ball->radius = 10.0;
    ball->active = false;
    ball->pos.x = WINDOW_WIDTH / 2;
    ball->pos.y = WINDOW_HEIGHT / 2;
    ball->speed.x = 0;
    ball->speed.y = -5;
}

void activate_ball(Ball *ball) {
    ball->pos.x += ball->speed.x;
    ball->pos.y += ball->speed.y;
}

void move_right(Player *player) {
    player->pos.x += 10;
}

void move_left(Player *player) {
    player->pos.x -= 10;
}

void update_game(Player player, Ball ball) {
    if (!player.is_game_over) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (matrix[i][j].is_visible) {
                    DrawRectangle(matrix[i][j].pos.x, matrix[i][j].pos.y, BRICK_WIDTH, BRICK_HEIGHT, BLACK);
                }
            }
        }
        DrawRectangle(player.pos.x, player.pos.y, PLAYER_WIDTH, PLAYER_HEIGHT, BLACK);
        DrawCircle(ball.pos.x, ball.pos.y, ball.radius, BLACK);

    } else {
        ClearBackground(BLACK);
    }
}

void detect_collisions(Ball *ball, Player *player, Sound deep, Music background_music) {
    if (ball->pos.y == 0) {
        ball->speed.y *= -1;
    }

    if (ball->pos.y == WINDOW_HEIGHT && !player->is_game_over) {
        StopMusicStream(background_music);
        player->is_game_over = true;
        PlaySound(deep);
        game_over_time = time(NULL);
    }

    if (ball->pos.x <= 0 || ball->pos.x >= WINDOW_WIDTH) {
        ball->speed.x *= -1;
    }

    Rectangle rec = {player->pos.x, player->pos.y, PLAYER_WIDTH, PLAYER_HEIGHT};
    if (CheckCollisionCircleRec(ball->pos, ball->radius, rec)) {
        ball->speed.y *= -1;
        ball->speed.x = (ball->pos.x - player->pos.x)/(PLAYER_WIDTH / 2)*5;
    }

    bool hasCollidedWithBrick = false;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            Rectangle brick = {matrix[i][j].pos.x, matrix[i][j].pos.y, BRICK_WIDTH, BRICK_HEIGHT};
            if (CheckCollisionCircleRec(ball->pos, ball->radius, brick) && matrix[i][j].is_visible) {
                matrix[i][j].is_visible = false;
                if (!hasCollidedWithBrick) {
                    ball->speed.y *= -1;
                    hasCollidedWithBrick = true;
                }
            }
        }
    }
}


bool check_win(Sound win_sound) {
    bool won = true;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (matrix[i][j].is_visible) {
                won = false;
            }
        }
    }

    return won;
}


int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Gachi Blocks");
    InitAudioDevice();
    SetMasterVolume(VOLUME);
    MaximizeWindow();

    Sound lose_sound = LoadSound("deep.mp3");
    Sound win_sound = LoadSound("stick_finger.mp3");
    Sound start_sound = LoadSound("onetwothree.mp3");

    Music background_music = LoadMusicStream("background_sound.mp3");

    SetTargetFPS(60);

    Player player = { 0 };
    Ball ball = { 0 };

    init_game(&player, &ball, start_sound);

    PlayMusicStream(background_music);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        activate_ball(&ball);

        UpdateMusicStream(background_music);

        if (IsKeyPressed(KEY_ENTER) && time(NULL) - game_over_time >= 2) {
            init_game(&player, &ball, start_sound);
            player.is_game_over = false;
            PlayMusicStream(background_music);
        }

        if (IsKeyDown(KEY_LEFT) && player.pos.x > 0) {
            move_left(&player);
        } else if (IsKeyDown(KEY_RIGHT) && player.pos.x < WINDOW_WIDTH - 70) {
            move_right(&player);
        }


        if (check_win(win_sound)) {
            player.is_game_over = true;
            StopMusicStream(background_music);
            PlaySound(win_sound);
            DrawText("You Won!", WINDOW_WIDTH / 2 - MeasureText("You Won!", 20) / 2, WINDOW_HEIGHT / 2 - 20 / 2, 20, WHITE);
        }

        detect_collisions(&ball, &player, lose_sound, background_music);
        update_game(player, ball);

        EndDrawing();
    }

    StopMusicStream(background_music);
    UnloadMusicStream(background_music);
    CloseWindow();
    
    return 0;
}