#ifndef SNAKE_GAME_CONFIG_H
#define SNAKE_GAME_CONFIG_H

#include <avr/io.h>

// SH1107 SPI Configuration
#define CS_PIN PB2   // D10
#define DC_PIN PB1   // D9
#define RES_PIN PB0  // D8

// Button Pins
#define UP_BTN_PIN PD2     // D2
#define DOWN_BTN_PIN PD3   // D3
#define LEFT_BTN_PIN PD4   // D4
#define RIGHT_BTN_PIN PD5  // D5

// Game Configuration
#define GRID_SIZE 16
#define CELL_SIZE 8
#define MAX_SNAKE_LENGTH 50
#define MOVE_DELAY 250
#define SCORE_AREA_HEIGHT 16
#define PARTITION_LINE_Y (SCORE_AREA_HEIGHT - 1)

#define INITIAL_SCORE 0
#define INITIAL_SNAKE_LENGTH 3

// Button Debouncing
#define DEBOUNCE_TIME 50

// Game State
typedef struct {
  uint8_t x;
  uint8_t y;
} Point;

#endif