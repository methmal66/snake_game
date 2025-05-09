/**
 * @file graphic.c
 * @brief Graphics rendering functions for SH1107 OLED display.
 */

#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include "config.h"
#include "display.h"
#include "font.h"
#include "types.h"

/**
 * @brief Draws a single pixel at specified coordinates.
 * @param x Horizontal position (0-127).
 * @param y Vertical position (0-63).
 */
void draw_pixel(uint8_t x, uint8_t y) {
  sh1107_page(y);
  sh1107_lowcol(x);
  sh1107_highcol(x);
  sh1107_data(y);
}

/**
 * @brief Clears an entire display page (8-pixel row group).
 * @param x Starting horizontal position (0-127).
 * @param page Page number (0-7 for 64px display).
 */
void clear_page(uint8_t x, uint8_t page) {
  sh1107_page(page);
  sh1107_lowcol(x);
  sh1107_highcol(x);
  sh1107_clean(0);
}

/**
 * @brief Draws a horizontal line across the display.
 * @param y Vertical position for the line (0-63).
 */
void draw_horizontal_line(uint8_t y) {
  for (uint8_t x = 0; x < 128; x++) {
    draw_pixel(x, y);
  }
}

/**
 * @brief Maps ASCII characters to font array indices.
 * @param c Character to look up.
 * @return Font array index (255 for invalid characters).
 */
uint8_t get_char_index(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else {
    switch (c) {
      case 'S':
        return CHAR_S_INDEX;
      case 'C':
        return CHAR_C_INDEX;
      case 'O':
        return CHAR_O_INDEX;
      case 'R':
        return CHAR_R_INDEX;
      case 'E':
        return CHAR_E_INDEX;
      case ':':
        return CHAR_COLON_INDEX;
      default:
        return CHAR_INVALID_INDEX;
    }
  }
}

/**
 * @brief Renders a character at specified coordinates.
 * @param x Horizontal starting position (0-127).
 * @param y Vertical starting position (0-63).
 * @param c Character to draw.
 */
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

/**
 * @brief Clears the score display area (top page).
 */
void clear_score_area() {
  for (uint8_t x = 0; x < 128; x++) {
    clear_page(x, 0);
  }
}

/**
 * @brief Draws a text label at specified coordinates.
 * @param x Horizontal starting position (0-127).
 * @param y Vertical starting position (0-63).
 * @param label Null-terminated string to draw.
 * @return Ending x-position after drawn text.
 */
uint8_t draw_label(uint8_t x, uint8_t y, const char* label) {
  uint8_t label_length = 0;
  for (uint8_t i = 0; label[i] != '\0'; i++) {
    label_length += x + i * FONT_WIDTH;
    draw_char(label_length, y, label[i]);
  }
  return label_length;
}

/**
 * @brief Draws/updates the score display.
 * @param score Pointer to current score value.
 */
void draw_score(uint16_t* score) {
  clear_score_area();
  uint8_t nextx = draw_label(0, 0, "SCORE:");

  char score_str[6];
  itoa(*score, score_str, 10);
  draw_label(nextx, 0, score_str);
}

/**
 * @brief Draws a circle using midpoint algorithm.
 * @param x0 Center x-coordinate (0-127).
 * @param y0 Center y-coordinate (0-63).
 * @param radius Circle radius in pixels.
 */
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

/**
 * @brief Clears the game play area (below partition line).
 */
void clear_play_area() {
  for (uint8_t page = PARTITION_LINE_Y / 8 + 1; page < 128; page += 8) {
    for (uint8_t x = 0; x < 128; x++) {
      clear_page(x, page);
    }
  }
}

/**
 * @brief Renders the snake on the display.
 * @param state Pointer to current GameState structure.
 */
void draw_snake(GameState* state) {
  for (uint8_t i = 0; i < state->snakeLength; i++) {
    uint8_t x = state->snake[i].x * CELL_SIZE + 1;
    uint8_t y = state->snake[i].y * CELL_SIZE + 1 + SCORE_AREA_HEIGHT;

    for (uint8_t dy = 0; dy < CELL_SIZE - 2; dy++) {
      for (uint8_t dx = 0; dx < CELL_SIZE - 2; dx++) {
        draw_pixel(x + dx, y + dy);
      }
    }
  }
}

/**
 * @brief Renders the food item on the display.
 * @param state Pointer to current GameState structure.
 */
void draw_food(GameState* state) {
  uint8_t foodX = state->food.x * CELL_SIZE + CELL_SIZE / 2;
  uint8_t foodY = state->food.y * CELL_SIZE + CELL_SIZE / 2 + SCORE_AREA_HEIGHT;
  draw_circle(foodX, foodY, CELL_SIZE / 2 - 1);
}