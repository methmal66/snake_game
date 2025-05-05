#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

#include "config.h"
#include "serial.h"

Point snake[MAX_SNAKE_LENGTH];
uint8_t snakeLength = 3;
Point food;
uint16_t score = 0;
volatile uint8_t direction = 0;  // 0=right, 1=down, 2=left, 3=up
volatile uint8_t gameOver = 0;
volatile uint32_t lastMoveTime = 0;

// Button Debouncing
volatile uint32_t lastButtonTime = 0;
volatile uint8_t buttonsEnabled = 1;

// ======================
// SH1107 Functions
// ======================
void sh1107_command(uint8_t cmd) {
  PORTB &= ~(1 << DC_PIN);
  PORTB &= ~(1 << CS_PIN);
  spi_write(cmd);
  PORTB |= (1 << CS_PIN);
}

void sh1107_data(uint8_t data) {
  PORTB |= (1 << DC_PIN);
  PORTB &= ~(1 << CS_PIN);
  spi_write(data);
  PORTB |= (1 << CS_PIN);
}

void sh1107_init() {
  // Hardware reset
  PORTB &= ~(1 << RES_PIN);
  _delay_ms(10);
  PORTB |= (1 << RES_PIN);
  _delay_ms(10);

  // Init sequence
  sh1107_command(0xAE);  // Display OFF
  sh1107_command(0xD5);
  sh1107_command(0x80);
  sh1107_command(0xA8);
  sh1107_command(0x7F);
  sh1107_command(0xD3);
  sh1107_command(0x00);
  sh1107_command(0x40);
  sh1107_command(0x8D);
  sh1107_command(0x14);
  sh1107_command(0x20);
  sh1107_command(0x00);
  sh1107_command(0xA1);
  sh1107_command(0xC8);
  sh1107_command(0xDA);
  sh1107_command(0x12);
  sh1107_command(0x81);
  sh1107_command(0xCF);
  sh1107_command(0xD9);
  sh1107_command(0xF1);
  sh1107_command(0xDB);
  sh1107_command(0x40);
  sh1107_command(0xA4);
  sh1107_command(0xA6);
  sh1107_command(0xAF);  // Display ON
}

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
void place_food() {
  uint8_t valid = 0;
  while (!valid) {
    food.x = rand() % GRID_SIZE;
    food.y = rand() % GRID_SIZE;
    valid = 1;
    for (uint8_t i = 0; i < snakeLength; i++) {
      if (snake[i].x == food.x && snake[i].y == food.y) {
        valid = 0;
        break;
      }
    }
  }
}

void move_snake() {
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
  for (uint8_t i = 1; i < snakeLength; i++) {
    if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
      gameOver = 1;
      return;
    }
  }

  // Food check
  if (newHead.x == food.x && newHead.y == food.y) {
    if (snakeLength < MAX_SNAKE_LENGTH) {
      snakeLength++;
    }
    score++;
    place_food();
  } else {
    // Move body
    for (uint8_t i = snakeLength - 1; i > 0; i--) {
      snake[i] = snake[i - 1];
    }
  }
  snake[0] = newHead;
}

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

void draw_score() {
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
  itoa(score, score_str, 10);
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

void render_game() {
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
  for (uint8_t i = 0; i < snakeLength; i++) {
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
  uint8_t foodX = food.x * CELL_SIZE + CELL_SIZE / 2;
  uint8_t foodY = food.y * CELL_SIZE + CELL_SIZE / 2 + SCORE_AREA_HEIGHT;
  draw_circle(foodX, foodY, CELL_SIZE / 2 - 1);

  // Update score display
  draw_score();
}

// ======================
// Main Program
// ======================
int main(void) {
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
  place_food();

  // Draw initial display
  draw_score();
  draw_horizontal_line(PARTITION_LINE_Y);
  render_game();

  // Main game loop
  while (1) {
    if (!gameOver) {
      // Using interrupt-based button handling

      if (lastMoveTime++ > MOVE_DELAY) {
        move_snake();
        lastMoveTime = 0;
        render_game();
      }
      _delay_ms(1);
    } else {
      // Game over - wait for any button
      if (!(PIND & ((1 << UP_BTN_PIN) | (1 << DOWN_BTN_PIN) |
                    (1 << LEFT_BTN_PIN) | (1 << RIGHT_BTN_PIN)))) {
        // Reset game
        snakeLength = 3;
        snake[0] = (Point){3, 4};
        snake[1] = (Point){2, 4};
        snake[2] = (Point){1, 4};
        direction = 0;
        score = 0;
        gameOver = 0;
        place_food();
        render_game();
      }
    }
  }
}