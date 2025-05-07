#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

#include "config.h"
#include "display.h"
#include "font.h"
#include "types.h"

// ======================
// Drawing Functions
// ======================

// Helper function to handle SH1107 page/column addressing efficiently
void draw_pixel(uint8_t x, uint8_t y) {
  sh1107_page(y);
  sh1107_lowcol(x);
  sh1107_highcol(x);
  sh1107_data(y);
}

void clear_page(uint8_t x, uint8_t page) {
  sh1107_page(page);
  sh1107_lowcol(x);
  sh1107_highcol(x);
  sh1107_clean(0);
}

void draw_horizontal_line(uint8_t y) {
  for (uint8_t x = 0; x < 128; x++) {
    draw_pixel(x, y);
  }
}

uint8_t get_char_index(char c) {
  uint8_t char_index;
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else {
    switch (c) {
      case 'S':
        return 10;
      case 'C':
        return 11;
      case 'O':
        return 12;
      case 'R':
        return 13;
      case 'E':
        return 14;
      case ':':
        return 15;
      default:
        return 255;  // Invalid character
    }
  }
}

void draw_char(uint8_t x, uint8_t y, char c) {
  static const uint8_t font[][5] = FONT;
  uint8_t char_index = get_char_index(c);

  for (uint8_t col = 0; col < 5; col++) {
    uint8_t c_page = font[char_index][col];
    sh1107_page(y);
    sh1107_lowcol(x + col);
    sh1107_highcol(x + col);
    sh1107_data(c_page << (y % 8));
  }
}

void clear_score_area() {
  for (uint8_t x = 0; x < 128; x++) {
    clear_page(x, 0);
  }
}

uint8_t draw_label(uint8_t x, uint8_t y, const char* label) {
  uint8_t label_length = 0;
  for (uint8_t i = 0; label[i] != '\0'; i++) {
    label_length += x + i * FONT_WIDTH;
    draw_char(label_length, y, label[i]);
  }
  return label_length;
}

void draw_score(uint16_t* score) {
  clear_score_area();
  uint8_t nextx = draw_label(0, 0, "SCORE:");

  char score_str[6];
  itoa(*score, score_str, 10);
  draw_label(nextx, 0, score_str);
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
  for (uint8_t page = PARTITION_LINE_Y / 8 + 1; page < 128; page += 8) {
    for (uint8_t x = 0; x < 128; x++) {
      clear_page(x, page);
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