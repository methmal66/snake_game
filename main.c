#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

#include "config.h"
#include "display.h"
#include "graphic.h"
#include "serial.h"

volatile uint8_t direction = 0;  // 0=right, 1=down, 2=left, 3=up
volatile uint8_t gameOver = 0;
volatile uint32_t lastMoveTime = 0;

// Button Debouncing
volatile uint32_t lastButtonTime = 0;
volatile uint8_t buttonsEnabled = 1;

// ======================
// Button Functions
// ======================
void init_buttons() {
  // Set as inputs with pull-ups
  DDRD &= ~((1 << UP_BTN_PIN) | (1 << DOWN_BTN_PIN) | (1 << LEFT_BTN_PIN) |
            (1 << RIGHT_BTN_PIN));
  PORTD |= (1 << UP_BTN_PIN) | (1 << DOWN_BTN_PIN) | (1 << LEFT_BTN_PIN) |
           (1 << RIGHT_BTN_PIN);

  // Enable pin change interrupts
  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21);
}

ISR(PCINT2_vect) {
  static uint32_t lastInterrupt = 0;
  uint32_t now = lastMoveTime;

  if (now - lastInterrupt < DEBOUNCE_TIME)
    return;
  lastInterrupt = now;

  uint8_t buttons = PIND;

  if (!(buttons & (1 << UP_BTN_PIN)) && direction != 1) {
    direction = 3;
  } else if (!(buttons & (1 << DOWN_BTN_PIN)) && direction != 3) {
    direction = 1;
  } else if (!(buttons & (1 << LEFT_BTN_PIN)) && direction != 0) {
    direction = 2;
  } else if (!(buttons & (1 << RIGHT_BTN_PIN)) && direction != 2) {
    direction = 0;
  }
}

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
                Point* food) {
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
      gameOver = 1;
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

// ======================
// Main Program
// ======================
int main(void) {
  uint16_t* score = malloc(sizeof(uint16_t));
  *score = INITIAL_SCORE;

  uint8_t* snakeLength = malloc(sizeof(uint8_t));
  *snakeLength = INITIAL_SNAKE_LENGTH;

  Point snake[MAX_SNAKE_LENGTH];
  Point* food;

  // Initialize hardware
  spi_init();
  init_buttons();
  sh1107_init();
  sei();  // Enable interrupts

  // Initialize game
  snake[0] = (Point){3, 4};
  snake[1] = (Point){2, 4};
  snake[2] = (Point){1, 4};
  direction = 0;  // Start moving right
  place_food(snakeLength, snake, food);

  // Draw initial display
  draw_score(score);
  draw_horizontal_line(PARTITION_LINE_Y);
  render_game(score, snakeLength, snake, food);

  // Main game loop
  while (1) {
    if (!gameOver) {
      // Using interrupt-based button handling

      if (lastMoveTime++ > MOVE_DELAY) {
        move_snake(score, snakeLength, snake, food);
        lastMoveTime = 0;
        render_game(score, snakeLength, snake, food);
      }
      _delay_ms(1);
    } else {
      // Game over - wait for any button
      if (!(PIND & ((1 << UP_BTN_PIN) | (1 << DOWN_BTN_PIN) |
                    (1 << LEFT_BTN_PIN) | (1 << RIGHT_BTN_PIN)))) {
        // Reset game
        *snakeLength = 3;
        snake[0] = (Point){3, 4};
        snake[1] = (Point){2, 4};
        snake[2] = (Point){1, 4};
        direction = 0;
        score = 0;
        gameOver = 0;
        place_food(snakeLength, snake, food);
        render_game(score, snakeLength, snake, food);
      }
    }
  }
}