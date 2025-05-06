#include <avr/io.h>
#include <util/delay.h>

#include "config.h"
#include "display.h"
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

void sh1107_data(uint8_t y) {
  uint8_t page_mask = 1 << (y % 8);
  PORTB |= (1 << DC_PIN);
  PORTB &= ~(1 << CS_PIN);
  spi_write(page_mask);
  PORTB |= (1 << CS_PIN);
}

void sh1107_clean(uint8_t y) {
  PORTB |= (1 << DC_PIN);
  PORTB &= ~(1 << CS_PIN);
  spi_write(0);
  PORTB |= (1 << CS_PIN);
}

void sh1107_page(uint8_t y) {
  uint8_t page = y / 8;
  sh1107_command(SH1107_SET_PAGE_ADDR + page);
}

void sh1107_lowcol(uint8_t x) {
  uint8_t low_nibble = (x & 0x0F);
  sh1107_command(SH1107_SET_LOW_COL_ADDR + low_nibble);
}

void sh1107_highcol(uint8_t x) {
  uint8_t high_nibble = ((x & 0xF0) >> 4);
  sh1107_command(SH1107_SET_HIGH_COL_ADDR + high_nibble);
}

void sh1107_init() {
  // Hardware reset
  PORTB &= ~(1 << RES_PIN);
  _delay_ms(10);
  PORTB |= (1 << RES_PIN);
  _delay_ms(10);

  // Init sequence
  sh1107_command(SH1107_DISPLAY_OFF);
  sh1107_command(SH1107_SET_CLOCK_DIV);
  sh1107_command(SH1107_CLOCK_DIV_DEFAULT);
  sh1107_command(SH1107_SET_MULTIPLEX_RATIO);
  sh1107_command(SH1107_MULTIPLEX_128);
  sh1107_command(SH1107_SET_DISPLAY_OFFSET);
  sh1107_command(SH1107_OFFSET_NONE);
  sh1107_command(SH1107_SET_START_LINE);
  sh1107_command(SH1107_CHARGE_PUMP_CTRL);
  sh1107_command(SH1107_CHARGE_PUMP_ENABLE);
  sh1107_command(SH1107_SET_ADDRESS_MODE);
  sh1107_command(SH1107_ADDRESS_MODE_HORIZ);
  sh1107_command(SH1107_SET_SEGMENT_REMAP);
  sh1107_command(SH1107_SET_COM_SCAN_DIR);
  sh1107_command(SH1107_SET_COM_PINS);
  sh1107_command(SH1107_COM_PINS_ALT);
  sh1107_command(SH1107_SET_CONTRAST);
  sh1107_command(SH1107_CONTRAST_DEFAULT);
  sh1107_command(SH1107_SET_PRECHARGE);
  sh1107_command(SH1107_PRECHARGE_DEFAULT);
  sh1107_command(SH1107_SET_VCOMH_DESELECT);
  sh1107_command(SH1107_VCOMH_DEFAULT);
  sh1107_command(SH1107_SET_ENTIRE_DISPLAY);
  sh1107_command(SH1107_SET_NORMAL_DISPLAY);
  sh1107_command(SH1107_DISPLAY_ON);
}