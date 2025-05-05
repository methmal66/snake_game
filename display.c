#include <avr/io.h>
#include <util/delay.h>

#include "config.h"
#include "serial.h"

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