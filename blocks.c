#include <raylib.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define PLAYER_WIDTH 70.0
#define PLAYER_HEIGHT 10.0
#define BRICK_WIDTH 80
#define BRICK_HEIGHT 20
#define VOLUME 150.0
#define COLORS 7


typedef struct {
    Vector2 pos;
    bool is_game_over;
    int lives;
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
    Color color;
} Brick;

Brick matrix[10][10];

Color MY_RED = { 230, 41, 55, 255 };
Color MY_ORANGE = { 255, 161, 0, 255 };
Color MY_YELLOW = { 253, 249, 0, 255 };
Color MY_GREEN = { 0, 228, 48, 255 };
Color MY_BLUE = { 0, 121, 241, 255 };
Color MY_INDIGO = { 75, 0, 130, 255 };
Color MY_VIOLET = { 135, 60, 190, 255 };

Color rainbow_colors[7];

void initialize_colors() {
    MY_RED = (Color){ 230, 41, 55, 255 };
    MY_ORANGE = (Color){ 255, 161, 0, 255 };
    MY_YELLOW = (Color){ 253, 249, 0, 255 };
    MY_GREEN = (Color){ 0, 228, 48, 255 };
    MY_BLUE = (Color){ 0, 121, 241, 255 };
    MY_INDIGO = (Color){ 75, 0, 130, 255 };
    MY_VIOLET = (Color){ 135, 60, 190, 255 };

    rainbow_colors[0] = MY_RED;
    rainbow_colors[1] = MY_ORANGE;
    rainbow_colors[2] = MY_YELLOW;
    rainbow_colors[3] = MY_GREEN;
    rainbow_colors[4] = MY_BLUE;
    rainbow_colors[5] = MY_INDIGO;
    rainbow_colors[6] = MY_VIOLET;
}

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

void init_game(Player *player, Ball *ball, Sound start_sound, bool full_restart) {
    start_game(start_sound);

    float x = 0; 
    float y = 0;

    if (full_restart) {
        for (int i = 0; i < 10; i++) {
            Brick brick = { 0 };
            for (int j = 0; j < 10; j++) {
                brick.pos.x = x;
                brick.pos.y = y;
                brick.is_visible = true;
                int random_index = GetRandomValue(0, COLORS - 1);
                brick.color = rainbow_colors[random_index];
                
                matrix[i][j] = brick;

                x += BRICK_WIDTH + 1; 
            }
            y += BRICK_HEIGHT + 1; 
            x = 0;
        }

        player->lives = 5;
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

void update_game(Player player, Ball ball, Texture2D heart, Texture2D loser, Sound win_sound, Music background_music) {
    if (!player.is_game_over && player.lives > 0) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (matrix[i][j].is_visible) {
                    DrawRectangle(matrix[i][j].pos.x, matrix[i][j].pos.y, BRICK_WIDTH, BRICK_HEIGHT, matrix[i][j].color);
                }
            }
        }
        DrawRectangle(player.pos.x, player.pos.y, PLAYER_WIDTH, PLAYER_HEIGHT, BLACK);
        DrawCircle(ball.pos.x, ball.pos.y, ball.radius, BLACK);

        int tex_x = 10;
        int tex_y = WINDOW_HEIGHT - 100;

        for (int i = 0; i < player.lives; i++) {
            DrawTexture(heart, tex_x, tex_y, (Color){200, 200, 200, 255});
            tex_x += heart.width;
        }
    } else {
        int total_points = 0;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (!matrix[i][j].is_visible) {
                    total_points++;
                }
            }
        }

        if (total_points == 100) {
            player.is_game_over = true;
            StopMusicStream(background_music);
            PlaySound(win_sound);
            DrawText("You Won!", WINDOW_WIDTH / 2 - MeasureText("You Won! Officially gei now...", 60) / 2, WINDOW_HEIGHT / 2 - 20 / 2, 20, BLACK);
            return;
        }

        int stripe_height = WINDOW_HEIGHT / COLORS;

        for (int i = 0; i < COLORS; i++) {
            DrawRectangle(0, i * stripe_height, WINDOW_WIDTH, stripe_height, rainbow_colors[i]);
        }

        DrawTexture(loser, WINDOW_WIDTH / 2 - loser.width / 2, WINDOW_HEIGHT / 2 - loser.height / 2, WHITE);
        char points_str[12];
        sprintf(points_str, "Points: %d", total_points);
        DrawText(points_str, WINDOW_WIDTH / 2 - MeasureText(points_str, 40) / 2, WINDOW_HEIGHT / 2 + loser.height / 2 + 10, 40, BLACK);
    }
}

void detect_collisions(Ball *ball, Player *player, Sound deep, Music background_music, Sound start_sound) {
    if (ball->pos.y == 0) {
        ball->speed.y *= -1;
    }

    if (ball->pos.y == WINDOW_HEIGHT - 50 && !player->is_game_over) {
        player->lives -= 1;
        if (player->lives > 0) {
            init_game(player, ball, start_sound, false);
        } else {
            StopMusicStream(background_music);
            PlaySound(deep);
        }
    }

    if (ball->pos.x <= 10 || ball->pos.x >= WINDOW_WIDTH - 10) {
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

bool check_win() {
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

    initialize_colors();

    Sound lose_sound = LoadSound("deep.mp3");
    Sound win_sound = LoadSound("stick_finger.mp3");
    Sound start_sound = LoadSound("onetwothree.mp3");

    Image heart = LoadImage("heart.png");
    ImageResize(&heart, 40, 40); 
    Texture2D texture_heart = LoadTextureFromImage(heart);          
    UnloadImage(heart);

    Image loser = LoadImage("loser.png");
    ImageResize(&loser, 300, 300); 
    Texture2D texture_loser = LoadTextureFromImage(loser);          
    UnloadImage(loser);

    Image icon = LoadImage("icon.png");
    SetWindowIcon(icon);

    Music background_music = LoadMusicStream("background_sound.mp3");

    SetTargetFPS(60);

    Player player = { 0 };
    Ball ball = { 0 };

    init_game(&player, &ball, start_sound, true);

    PlayMusicStream(background_music);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground((Color){200, 200, 200, 255});
        
        activate_ball(&ball);

        UpdateMusicStream(background_music);

        if (IsKeyPressed(KEY_ENTER)) {
            init_game(&player, &ball, start_sound, true);
            player.is_game_over = false;
            PlayMusicStream(background_music);
        }

        if (IsKeyDown(KEY_LEFT) && player.pos.x > 10) {
            move_left(&player);
        } else if (IsKeyDown(KEY_RIGHT) && player.pos.x < WINDOW_WIDTH - 80) {
            move_right(&player);
        }

        detect_collisions(&ball, &player, lose_sound, background_music, start_sound);
        update_game(player, ball, texture_heart, texture_loser, win_sound, background_music);

        EndDrawing();
    }

    StopMusicStream(background_music);
    UnloadMusicStream(background_music);
    CloseWindow();
    
    return 0;
}