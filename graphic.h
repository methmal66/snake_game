/**
 * @file graphic.h
 * @brief Header file for graphics rendering functions for the Snake game.
 */

#ifndef SNAKE_GAME_GRAPHIC_H
#define SNAKE_GAME_GRAPHIC_H

#include <stdint.h>
#include "types.h"

void draw_horizontal_line(uint8_t);

void clear_page(uint8_t, uint8_t);

uint8_t get_char_index(char);

void draw_char(uint8_t, uint8_t, char);

void draw_label(uint8_t, uint8_t, char*);

void clear_score_area();

void draw_score(uint16_t*);

void clear_play_area();

void draw_snake(GameState*);

void draw_food(GameState*);

void draw_pixel(uint8_t, uint8_t);

void draw_circle(uint8_t, uint8_t, uint8_t);

#endif
