/**
 * @file main.c
 * @brief Main game loop and hardware initialization for Snake game.
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "config.h"
#include "display.h"
#include "game.h"
#include "graphic.h"
#include "serial.h"
#include "types.h"

volatile uint32_t lastMoveTime = 0;    // Timestamp of last snake movement
volatile uint32_t lastButtonTime = 0;  // Timestamp for button debouncing
volatile uint8_t buttonsEnabled = 1;   // Button input enable flag
volatile uint8_t direction = INITIAL_DIRECTION;  // Current snake direction

/**
 * @brief Initializes button inputs and enables pin change interrupts.
 * @note Configures:
 * - UP/DOWN/LEFT/RIGHT buttons as inputs with pull-ups
 * - PCINT18-21 interrupts for button pins
 */
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

/**
 * @brief Initializes all hardware peripherals.
 * @note Enables:
 * - SPI interface
 * - Button inputs
 * - SH1107 display
 * - Global interrupts
 */
void hardware_init() {
  spi_init();
  init_buttons();
  sh1107_init();
  sei();  // Enable global interrupts
}

/**
 * @brief Main game entry point.
 * @note Implements:
 * - Game state initialization
 * - Main game loop with movement timing
 * - Game over detection and reset handling
 */
int main(void) {
  GameState* state;
  state->direction = &direction;
  state->score = INITIAL_SCORE;
  state->snakeLength = INITIAL_SNAKE_LENGTH;

  hardware_init();
  reset_game(state);

  // Main game loop
  while (1) {
    if (!state->gameOver) {
      // Using interrupt-based button handling
      if (lastMoveTime++ > MOVE_DELAY) {
        move_snake(state);
        lastMoveTime = 0;
        render_game(state);
      }
      _delay_ms(1);
    } else {
      // Game over - wait for any button press to reset
      if (!(PIND & ((1 << UP_BTN_PIN) | (1 << DOWN_BTN_PIN) |
                    (1 << LEFT_BTN_PIN) | (1 << RIGHT_BTN_PIN)))) {
        reset_game(state);
      }
    }
  }
}

/**
 * @brief Pin Change Interrupt handler for button inputs.
 * @note Implements:
 * - Debouncing (DEBOUNCE_TIME)
 * - Direction change logic (prevents 180° turns)
 */
ISR(PCINT2_vect) {
  static uint32_t lastInterrupt = 0;
  uint32_t now = lastMoveTime;

  if (now - lastInterrupt < DEBOUNCE_TIME)
    return;
  lastInterrupt = now;

  uint8_t buttons = PIND;

  if (!(buttons & (1 << UP_BTN_PIN)) && direction != DIRECTION_DOWN) {
    direction = DIRECTION_UP;
  } else if (!(buttons & (1 << DOWN_BTN_PIN)) && direction != DIRECTION_UP) {
    direction = DIRECTION_DOWN;
  } else if (!(buttons & (1 << LEFT_BTN_PIN)) && direction != DIRECTION_RIGHT) {
    direction = DIRECTION_LEFT;
  } else if (!(buttons & (1 << RIGHT_BTN_PIN)) && direction != DIRECTION_LEFT) {
    direction = DIRECTION_RIGHT;
  }
}