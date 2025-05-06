#ifndef SNAKE_GAME_GRAPHIC_H
#define SNAKE_GAME_GRAPHIC_H

#include <stdint.h>
#include "types.h"

void draw_horizontal_line(uint8_t);

void draw_char(uint8_t, uint8_t, char);

void draw_score(uint16_t*);

void draw_pixel(uint8_t, uint8_t);

void draw_circle(uint8_t, uint8_t, uint8_t);

void render_game(GameState*);

#endif
