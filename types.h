#ifndef SNAKE_GAME_TYPES_H
#define SNAKE_GAME_TYPES_H

#include <stdint.h>

typedef struct {
  uint8_t x;
  uint8_t y;
} Point;

typedef struct {
  uint16_t score;
  uint8_t snakeLength;
  Point* snake;
  uint8_t gameOver;
  Point food;
  volatile uint8_t* direction;
} GameState;

#endif