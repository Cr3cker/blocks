# Blocks Game

Blocks Game is a simple and fun game built using the Raylib library in C. The game is similar to the classic game of Breakout, where you control a paddle to bounce a ball and destroy bricks.

## Game Controls

- **Left Arrow Key**: Move the paddle to the left.
- **Right Arrow Key**: Move the paddle to the right.
- **Enter Key**: Restart the game.

## Game Rules

- The game starts with a grid of bricks at the top of the screen.
- The player controls a paddle at the bottom of the screen by moving it left or right.
- A ball moves around the screen, bouncing off the top and sides.
- When the ball hits a brick, the brick is destroyed and the ball bounces back.
- If the ball hits the bottom of the screen, the game is over.
- The goal is to destroy all the bricks without letting the ball hit the bottom of the screen.

## Building and Running the Game

To build and run the game, you need to have the Raylib library installed on your system. Once you have it installed, you can use a compiler like `gcc` to compile the game:

```bash
gcc -o blocks blocks.c -lraylib
```

Then, you can run the game:

```bash
./blocks
```