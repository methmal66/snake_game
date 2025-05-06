#include <stdlib.h>

#include "config.h"
#include "display.h"
#include "graphic.h"
#include "types.h"

// ======================
// Game Functions
// ======================
void place_food(uint8_t* snakeLength, Point* snake, Point* food) {
  uint8_t valid = 0;
  while (!valid) {
    food->x = rand() % GRID_SIZE;
    food->y = rand() % GRID_SIZE;
    valid = 1;
    for (uint8_t i = 0; i < *snakeLength; i++) {
      if (snake[i].x == food->x && snake[i].y == food->y) {
        valid = 0;
        break;
      }
    }
  }
}

void move_snake(uint16_t* score,
                uint8_t* snakeLength,
                Point* snake,
                Point* food,
                uint8_t direction,
                uint8_t* gameOver) {
  Point newHead = snake[0];

  switch (direction) {
    case 0:
      newHead.x++;
      break;  // Right
    case 1:
      newHead.y++;
      break;  // Down
    case 2:
      newHead.x--;
      break;  // Left
    case 3:
      newHead.y--;
      break;  // Up
  }

  // Wrap around boundaries
  newHead.x %= GRID_SIZE;
  newHead.y %= GRID_SIZE;

  // Collision check with self
  for (uint8_t i = 1; i < *snakeLength; i++) {
    if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
      *gameOver = 1;
      return;
    }
  }

  // Food check
  if (newHead.x == food->x && newHead.y == food->y) {
    if (*snakeLength < MAX_SNAKE_LENGTH) {
      snakeLength++;
    }
    *score++;
    place_food(snakeLength, snake, food);
  } else {
    // Move body
    for (uint8_t i = *snakeLength - 1; i > 0; i--) {
      snake[i] = snake[i - 1];
    }
  }
  snake[0] = newHead;
}

void reset_game(uint8_t* snakeLength,
                Point* snake,
                volatile uint8_t* direction,
                uint16_t* score,
                uint8_t* gameOver,
                Point* food) {
  *snakeLength = 3;
  snake[0] = (Point){3, 4};
  snake[1] = (Point){2, 4};
  snake[2] = (Point){1, 4};
  *direction = INITIAL_DIRECTION;
  *score = INITIAL_SCORE;
  *gameOver = 0;

  place_food(snakeLength, snake, food);
  draw_score(score);
  draw_horizontal_line(PARTITION_LINE_Y);
  render_game(score, snakeLength, snake, food);
}