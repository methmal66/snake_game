#ifndef SNAKE_GAME_GAME_H
#define SNAKE_GAME_GAME_H

#include "types.h"

void render_game(GameState*);

void place_food(GameState*);

uint8_t check_collision(GameState*);

Point calculate_new_head(GameState*);

void handle_food_check(GameState*, Point);

void move_snake(GameState*);

void reset_game(GameState*);

#endif