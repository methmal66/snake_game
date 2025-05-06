#ifndef SNAKE_GAME_GAME_H
#define SNAKE_GAME_GAME_H

#include "types.h"

void place_food(uint8_t*, Point*, Point*);

void move_snake(uint16_t*, uint8_t*, Point*, Point*, uint8_t, uint8_t*);

void reset_game(uint8_t*,
                Point*,
                volatile uint8_t*,
                uint16_t*,
                uint8_t*,
                Point*);

#endif