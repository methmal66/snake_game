#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

#include "config.h"
#include "display.h"
#include "types.h"

// ======================
// Drawing Functions
// ======================
void draw_horizontal_line(uint8_t y) {
  for (uint8_t col = 0; col < 128; col++) {
    sh1107_page(y);
    sh1107_lowcol(col);
    sh1107_highcol(col);
    sh1107_data(y);
  }
}

void draw_char(uint8_t x, uint8_t y, char c) {
  static const uint8_t font[][5] = {
      {0x3E, 0x51, 0x49, 0x45, 0x3E},  // 0
      {0x00, 0x42, 0x7F, 0x40, 0x00},  // 1
      {0x42, 0x61, 0x51, 0x49, 0x46},  // 2
      {0x21, 0x41, 0x45, 0x4B, 0x31},  // 3
      {0x18, 0x14, 0x12, 0x7F, 0x10},  // 4
      {0x27, 0x45, 0x45, 0x45, 0x39},  // 5
      {0x3C, 0x4A, 0x49, 0x49, 0x30},  // 6
      {0x01, 0x71, 0x09, 0x05, 0x03},  // 7
      {0x36, 0x49, 0x49, 0x49, 0x36},  // 8
      {0x06, 0x49, 0x49, 0x29, 0x1E},  // 9
      {0x26, 0x49, 0x49, 0x49, 0x32},  // S
      {0x3E, 0x41, 0x41, 0x41, 0x22},  // C
      {0x3E, 0x41, 0x41, 0x41, 0x3E},  // O
      {0x7F, 0x09, 0x19, 0x29, 0x46},  // R
      {0x7F, 0x49, 0x49, 0x49, 0x41},  // E
      {0x00, 0x36, 0x36, 0x00, 0x00}   // :
  };

  uint8_t char_index;
  if (c >= '0' && c <= '9') {
    char_index = c - '0';
  } else {
    switch (c) {
      case 'S':
        char_index = 10;
        break;
      case 'C':
        char_index = 11;
        break;
      case 'O':
        char_index = 12;
        break;
      case 'R':
        char_index = 13;
        break;
      case 'E':
        char_index = 14;
        break;
      case ':':
        char_index = 15;
        break;
      default:
        return;
    }
  }

  for (uint8_t col = 0; col < 5; col++) {
    sh1107_page(y);
    sh1107_lowcol(x + col);
    sh1107_highcol(x + col);
    sh1107_data(font[char_index][col] << (y % 8));
  }
}

void draw_score(uint16_t* score) {
  // Clear score area
  for (uint8_t col = 0; col < 128; col++) {
    sh1107_page(0);
    sh1107_lowcol(col);
    sh1107_highcol(col);
    sh1107_clean(0);
  }

  // Draw "SCORE:" label
  draw_char(0, 0, 'S');
  draw_char(6, 0, 'C');
  draw_char(12, 0, 'O');
  draw_char(18, 0, 'R');
  draw_char(24, 0, 'E');
  draw_char(30, 0, ':');

  // Draw score value
  char score_str[6];
  itoa(*score, score_str, 10);
  for (uint8_t i = 0; score_str[i] != '\0'; i++) {
    draw_char(36 + i * 6, 0, score_str[i]);
  }
}

// Helper function to handle SH1107 page/column addressing efficiently
void draw_pixel(uint8_t x, uint8_t y) {
  sh1107_page(y);
  sh1107_lowcol(x);
  sh1107_highcol(x);
  sh1107_data(y);
}

void draw_circle(uint8_t x0, uint8_t y0, uint8_t radius) {
  int16_t f = 1 - radius;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * radius;
  int16_t x = 0;
  int16_t y = radius;

  // Draw initial 4 cardinal points (optimized)
  draw_pixel(x0, y0 + radius);
  draw_pixel(x0, y0 - radius);
  draw_pixel(x0 + radius, y0);
  draw_pixel(x0 - radius, y0);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    // Draw all 8 symmetric points
    draw_pixel(x0 + x, y0 + y);
    draw_pixel(x0 + x, y0 - y);
    draw_pixel(x0 - x, y0 + y);
    draw_pixel(x0 - x, y0 - y);
    draw_pixel(x0 + y, y0 + x);
    draw_pixel(x0 + y, y0 - x);
    draw_pixel(x0 - y, y0 + x);
    draw_pixel(x0 - y, y0 - x);
  }

  // Draw 45° points if missed (when x == y)
  if (x == y) {
    draw_pixel(x0 + x, y0 + y);
    draw_pixel(x0 + x, y0 - y);
    draw_pixel(x0 - x, y0 + y);
    draw_pixel(x0 - x, y0 - y);
  }
}

void render_game(GameState* state) {
  // Clear play area only (below partition line)
  for (uint8_t y = PARTITION_LINE_Y / 8 + 1; y < 128; y += 8) {
    for (uint8_t col = 0; col < 128; col++) {
      sh1107_page(y);
      sh1107_lowcol(col);
      sh1107_highcol(col);
      sh1107_clean(0);
    }
  }

  // Draw partition line
  draw_horizontal_line(PARTITION_LINE_Y);

  // Draw snake
  for (uint8_t i = 0; i < state->snakeLength; i++) {
    uint8_t x = state->snake[i].x * CELL_SIZE + 1;
    uint8_t y = state->snake[i].y * CELL_SIZE + 1 + SCORE_AREA_HEIGHT;
    for (uint8_t dy = 0; dy < CELL_SIZE - 2; dy++) {
      for (uint8_t dx = 0; dx < CELL_SIZE - 2; dx++) {
        draw_pixel(x + dx, y + dy);
      }
    }
  }

  // Draw food
  uint8_t foodX = state->food.x * CELL_SIZE + CELL_SIZE / 2;
  uint8_t foodY = state->food.y * CELL_SIZE + CELL_SIZE / 2 + SCORE_AREA_HEIGHT;
  draw_circle(foodX, foodY, CELL_SIZE / 2 - 1);

  // Update score display
  draw_score(&(state->score));
}