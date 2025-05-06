#include <stdlib.h>

#include "config.h"
#include "display.h"
#include "graphic.h"
#include "types.h"

// ======================
// Game Functions
// ======================
void place_food(GameState* state) {
  uint8_t valid = 0;
  while (!valid) {
    state->food.x = rand() % GRID_SIZE;
    state->food.y = rand() % GRID_SIZE;
    valid = 1;
    for (uint8_t i = 0; i < state->snakeLength; i++) {
      if (state->snake[i].x == state->food.x &&
          state->snake[i].y == state->food.y) {
        valid = 0;
        break;
      }
    }
  }
}

void move_snake(GameState* state) {
  Point newHead = state->snake[0];

  switch (state->direction) {
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
  for (uint8_t i = 1; i < state->snakeLength; i++) {
    if (state->snake[i].x == newHead.x && state->snake[i].y == newHead.y) {
      state->gameOver = 1;
      return;
    }
  }

  // Food check
  if (newHead.x == state->food.x && newHead.y == state->food.y) {
    if (state->snakeLength < MAX_SNAKE_LENGTH) {
      state->snakeLength++;
    }
    state->score++;
    place_food(state);
  } else {
    // Move body
    for (uint8_t i = state->snakeLength - 1; i > 0; i--) {
      state->snake[i] = state->snake[i - 1];
    }
  }
  state->snake[0] = newHead;
}

void reset_game(GameState* state) {
  state->direction = INITIAL_DIRECTION;
  state->score = INITIAL_SCORE;
  state->gameOver = 0;

  Point snake_[MAX_SNAKE_LENGTH];
  snake_[0] = (Point){3, 4};
  snake_[1] = (Point){2, 4};
  snake_[2] = (Point){1, 4};
  state->snake = snake_;

  place_food(state);
  draw_score(&(state->score));
  draw_horizontal_line(PARTITION_LINE_Y);
  render_game(state);
}