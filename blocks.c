#include <raylib.h>
#include <stdbool.h>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define PLAYER_WIDTH 70.0
#define PLAYER_HEIGHT 10.0
#define BRICK_WIDTH 10
#define BRICK_HEIGHT 5


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
    Vector2 size;
} Brick;

Brick matrix[10][10];

void init_game(Player *player, Ball *ball) {
    float x = 0;
    float y = 0;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            Brick brick = { 0 };
            brick.pos.x = x;
            brick.pos.y = y;
            matrix[i][j] = brick;

            x += BRICK_WIDTH;
        }
        y += BRICK_HEIGHT;
    }

    player->pos.x = WINDOW_WIDTH / 2;
    player->pos.y = WINDOW_HEIGHT - 200;
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
    player->pos.x += 5;
}

void move_left(Player *player) {
    player->pos.x -= 5;
}

void update_game(Player player, Ball ball) {
    if (!player.is_game_over) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                DrawRectangle(matrix[i][j].pos.x, matrix[i][j].pos.y, BRICK_WIDTH, BRICK_HEIGHT, BLACK);
            }
        }
        DrawRectangle(player.pos.x, player.pos.y, PLAYER_WIDTH, PLAYER_HEIGHT, BLACK);
        DrawCircle(ball.pos.x, ball.pos.y, ball.radius, BLACK);

    } else {
        DrawText("Game over!", WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 20, BLACK);
    }
}

void detect_collisions(Ball *ball, Player *player) {
    if (ball->pos.y == 0) {
        ball->speed.y *= -1;
    }

    if (ball->pos.y == WINDOW_HEIGHT) {
        player->is_game_over = true;
    }

    if (ball->pos.x <= 0 || ball->pos.x >= WINDOW_WIDTH) {
        ball->speed.x *= -1;
    }

    Rectangle rec = {player->pos.x, player->pos.y, PLAYER_WIDTH, PLAYER_HEIGHT};
    if (CheckCollisionCircleRec(ball->pos, ball->radius, rec)) {
        ball->speed.y *= -1;
        ball->speed.x = (ball->pos.x - player->pos.x)/(PLAYER_WIDTH / 2)*5;
    }
}


int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Blocks game");
    MaximizeWindow();

    SetTargetFPS(60);

    Player player = { 0 };
    Ball ball = { 0 };

    init_game(&player, &ball);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        activate_ball(&ball);

        if (IsKeyPressed(KEY_ENTER))
        {
            init_game(&player, &ball);
            player.is_game_over = false;
        }

        if (IsKeyDown(KEY_LEFT) && player.pos.x > 0) {
            move_left(&player);
        } else if (IsKeyDown(KEY_RIGHT) && player.pos.x < WINDOW_WIDTH - 50) {
            move_right(&player);
        }


        detect_collisions(&ball, &player);
        update_game(player, ball);

        EndDrawing();
    }

    CloseWindow();
    
    return 0;
}