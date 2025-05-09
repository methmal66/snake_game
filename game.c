/**
 * @file game.c
 * @brief Core game logic for Snake game implementation.
 */

#include <stdlib.h>
#include "config.h"
#include "display.h"
#include "graphic.h"
#include "types.h"

/**
 * @brief Renders the complete game state on the display.
 * @param state Pointer to the current GameState structure.
 */
void render_game(GameState* state) {
  clear_play_area();
  draw_horizontal_line(PARTITION_LINE_Y);
  draw_snake(state);
  draw_food(state);
  draw_score(&(state->score));
}

/**
 * @brief Places food at a random valid position on the grid.
 * @param state Pointer to the current GameState structure.
 * @note Ensures food doesn't spawn on snake segments.
 */
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

/**
 * @brief Calculates the snake's new head position based on current direction.
 * @param state Pointer to the current GameState structure.
 * @return Point structure containing the new head coordinates.
 * @note Implements screen wrapping via modulo operation.
 */
Point calculate_new_head(GameState* state) {
  Point newHead = state->snake[0];

  switch (*(state->direction)) {
    case DIRECTION_RIGHT:
      newHead.x++;
      break;
    case DIRECTION_DOWN:
      newHead.y++;
      break;
    case DIRECTION_LEFT:
      newHead.x--;
      break;
    case DIRECTION_UP:
      newHead.y--;
      break;
  }

  // Wrap around boundaries
  newHead.x %= GRID_SIZE;
  newHead.y %= GRID_SIZE;

  return newHead;
}

/**
 * @brief Checks if new head position collides with snake body.
 * @param state Pointer to the current GameState structure.
 * @param newHead Proposed new head position to check.
 * @return 1 if collision detected, 0 otherwise.
 */
uint8_t check_collision(GameState* state, Point newHead) {
  for (uint8_t i = 1; i < state->snakeLength; i++) {
    if (state->snake[i].x == newHead.x && state->snake[i].y == newHead.y) {
      return 1;
    }
  }
  return 0;
}

/**
 * @brief Handles food consumption and score updates.
 * @param state Pointer to the current GameState structure.
 * @param newHead Current head position to check against food.
 */
void handle_food_check(GameState* state, Point newHead) {
  if (newHead.x == state->food.x && newHead.y == state->food.y) {
    if (state->snakeLength < MAX_SNAKE_LENGTH) {
      state->snakeLength++;
    }
    state->score++;
    place_food(state);
  }
}

/**
 * @brief Moves all snake body segments forward.
 * @param state Pointer to the current GameState structure.
 * @note Shifts each segment to the position of the preceding segment.
 */
void move_snake_body(GameState* state) {
  for (uint8_t i = state->snakeLength - 1; i > 0; i--) {
    state->snake[i] = state->snake[i - 1];
  }
}

/**
 * @brief Coordinates complete snake movement and collision handling.
 * @param state Pointer to the current GameState structure.
 * @note Sets gameOver flag if collision occurs with body.
 */
void move_snake(GameState* state) {
  Point newHead = calculate_new_head(state);

  if (check_collision(state, newHead)) {
    state->gameOver = 1;
    return;
  }

  uint8_t ateFood = (newHead.x == state->food.x && newHead.y == state->food.y);
  handle_food_check(state, newHead);

  if (!ateFood) {
    move_snake_body(state);
  }

  state->snake[0] = newHead;
}

/**
 * @brief Resets game state to initial conditions.
 * @param state Pointer to the GameState structure to reset.
 * @note Reinitializes snake position, score, and spawns new food.
 */
void reset_game(GameState* state) {
  state->direction = INITIAL_DIRECTION;
  state->score = INITIAL_SCORE;
  state->gameOver = 0;
  state->snake = (Point[MAX_SNAKE_LENGTH])INITIAL_SNAKE;

  place_food(state);
  draw_score(&(state->score));
  draw_horizontal_line(PARTITION_LINE_Y);
  render_game(state);
}