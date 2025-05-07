#ifndef SNAKE_GAME_GAME_H
#define SNAKE_GAME_GAME_H

#include "types.h"

void render_game(GameState*);

void place_food(GameState*);

void move_snake(GameState*);

void reset_game(GameState*);

#endif