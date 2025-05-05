#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

#include "config.h"
#include "display.h"

// ======================
// Drawing Functions
// ======================
void draw_horizontal_line(uint8_t y) {
  sh1107_command(0xB0 + (y / 8));
  sh1107_command(0x00);
  sh1107_command(0x10);
  for (uint8_t col = 0; col < 128; col++) {
    sh1107_data(1 << (y % 8));
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
    sh1107_command(0xB0 + (y / 8));
    sh1107_command(0x00 + ((x + col) & 0x0F));
    sh1107_command(0x10 + (((x + col) & 0xF0) >> 4));
    sh1107_data(font[char_index][col] << (y % 8));
  }
}

void draw_score(uint16_t* score) {
  // Clear score area
  sh1107_command(0xB0);
  sh1107_command(0x00);
  sh1107_command(0x10);
  for (uint8_t col = 0; col < 128; col++) {
    sh1107_data(0x00);
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

void draw_circle(uint8_t x0, uint8_t y0, uint8_t radius) {
  int16_t f = 1 - radius;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * radius;
  int16_t x = 0;
  int16_t y = radius;

  // Draw the 8 circle points
  sh1107_command(0xB0 + ((y0 + radius) / 8));
  sh1107_command(0x00 + ((x0) & 0x0F));
  sh1107_command(0x10 + (((x0) & 0xF0) >> 4));
  sh1107_data(1 << ((y0 + radius) % 8));

  sh1107_command(0xB0 + ((y0 - radius) / 8));
  sh1107_command(0x00 + ((x0) & 0x0F));
  sh1107_command(0x10 + (((x0) & 0xF0) >> 4));
  sh1107_data(1 << ((y0 - radius) % 8));

  sh1107_command(0xB0 + ((y0) / 8));
  sh1107_command(0x00 + ((x0 + radius) & 0x0F));
  sh1107_command(0x10 + (((x0 + radius) & 0xF0) >> 4));
  sh1107_data(1 << ((y0) % 8));

  sh1107_command(0xB0 + ((y0) / 8));
  sh1107_command(0x00 + ((x0 - radius) & 0x0F));
  sh1107_command(0x10 + (((x0 - radius) & 0xF0) >> 4));
  sh1107_data(1 << ((y0) % 8));

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    // Draw 8 circle points
    sh1107_command(0xB0 + ((y0 + y) / 8));
    sh1107_command(0x00 + ((x0 + x) & 0x0F));
    sh1107_command(0x10 + (((x0 + x) & 0xF0) >> 4));
    sh1107_data(1 << ((y0 + y) % 8));

    sh1107_command(0xB0 + ((y0 - y) / 8));
    sh1107_command(0x00 + ((x0 + x) & 0x0F));
    sh1107_command(0x10 + (((x0 + x) & 0xF0) >> 4));
    sh1107_data(1 << ((y0 - y) % 8));

    sh1107_command(0xB0 + ((y0 + y) / 8));
    sh1107_command(0x00 + ((x0 - x) & 0x0F));
    sh1107_command(0x10 + (((x0 - x) & 0xF0) >> 4));
    sh1107_data(1 << ((y0 + y) % 8));

    sh1107_command(0xB0 + ((y0 - y) / 8));
    sh1107_command(0x00 + ((x0 - x) & 0x0F));
    sh1107_command(0x10 + (((x0 - x) & 0xF0) >> 4));
    sh1107_data(1 << ((y0 - y) % 8));

    sh1107_command(0xB0 + ((y0 + x) / 8));
    sh1107_command(0x00 + ((x0 + y) & 0x0F));
    sh1107_command(0x10 + (((x0 + y) & 0xF0) >> 4));
    sh1107_data(1 << ((y0 + x) % 8));

    sh1107_command(0xB0 + ((y0 - x) / 8));
    sh1107_command(0x00 + ((x0 + y) & 0x0F));
    sh1107_command(0x10 + (((x0 + y) & 0xF0) >> 4));
    sh1107_data(1 << ((y0 - x) % 8));

    sh1107_command(0xB0 + ((y0 + x) / 8));
    sh1107_command(0x00 + ((x0 - y) & 0x0F));
    sh1107_command(0x10 + (((x0 - y) & 0xF0) >> 4));
    sh1107_data(1 << ((y0 + x) % 8));

    sh1107_command(0xB0 + ((y0 - x) / 8));
    sh1107_command(0x00 + ((x0 - y) & 0x0F));
    sh1107_command(0x10 + (((x0 - y) & 0xF0) >> 4));
    sh1107_data(1 << ((y0 - x) % 8));
  }
}

void render_game(uint16_t* score,
                 uint8_t* snakeLength,
                 Point* snake,
                 Point* food) {
  // Clear play area only (below partition line)
  for (uint8_t page = PARTITION_LINE_Y / 8 + 1; page < 16; page++) {
    sh1107_command(0xB0 + page);
    sh1107_command(0x00);
    sh1107_command(0x10);
    for (uint8_t col = 0; col < 128; col++) {
      sh1107_data(0x00);
    }
  }

  // Draw partition line
  draw_horizontal_line(PARTITION_LINE_Y);

  // Draw snake
  for (uint8_t i = 0; i < *snakeLength; i++) {
    uint8_t x = snake[i].x * CELL_SIZE + 1;
    uint8_t y = snake[i].y * CELL_SIZE + 1 + SCORE_AREA_HEIGHT;
    for (uint8_t dy = 0; dy < CELL_SIZE - 2; dy++) {
      for (uint8_t dx = 0; dx < CELL_SIZE - 2; dx++) {
        sh1107_command(0xB0 + ((y + dy) / 8));
        sh1107_command(0x00 + ((x + dx) & 0x0F));
        sh1107_command(0x10 + (((x + dx) & 0xF0) >> 4));
        sh1107_data(1 << ((y + dy) % 8));
      }
    }
  }

  // Draw food
  uint8_t foodX = food->x * CELL_SIZE + CELL_SIZE / 2;
  uint8_t foodY = food->y * CELL_SIZE + CELL_SIZE / 2 + SCORE_AREA_HEIGHT;
  draw_circle(foodX, foodY, CELL_SIZE / 2 - 1);

  // Update score display
  draw_score(score);
}