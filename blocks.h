#ifndef GAME_H
#define GAME_H

#include <raylib.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define PLAYER_WIDTH 70.0
#define PLAYER_HEIGHT 10.0
#define BRICK_WIDTH 80
#define BRICK_HEIGHT 20
#define VOLUME 150.0
#define COLORS 7
#define INIT_NUM_BALLS 1

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
    bool bonus;
} Brick;

#endif