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

volatile uint32_t lastMoveTime = 0;

// Button Debouncing
volatile uint32_t lastButtonTime = 0;
volatile uint8_t buttonsEnabled = 1;

volatile uint8_t direction = INITIAL_DIRECTION;

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

void hardware_init() {
  spi_init();
  init_buttons();
  sh1107_init();
  sei();
}

// ======================
// Main Program
// ======================
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
      // Game over - wait for any button
      if (!(PIND & ((1 << UP_BTN_PIN) | (1 << DOWN_BTN_PIN) |
                    (1 << LEFT_BTN_PIN) | (1 << RIGHT_BTN_PIN)))) {
        // Reset game
        reset_game(state);
      }
    }
  }
}