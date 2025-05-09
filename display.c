/**
 * @file display.c
 * @brief SH1107 OLED display driver (AVR-compatible).
 */
#include <avr/io.h>
#include <util/delay.h>

#include "config.h"
#include "display.h"
#include "serial.h"

/**
 * @brief Sends a command byte to the SH1107 display.
 * @param cmd The command byte to send.
 */
void sh1107_command(uint8_t cmd) {
  PORTB &= ~(1 << DC_PIN);  // DC low for command mode
  PORTB &= ~(1 << CS_PIN);  // CS low to enable SPI
  spi_write(cmd);           // Transmit command
  PORTB |= (1 << CS_PIN);   // CS high to end transaction
}

/**
 * @brief Sends a data byte to the SH1107 display for pixel rendering.
 * @param y The vertical position (0-127) to target.
 */
void sh1107_data(uint8_t y) {
  uint8_t page_mask = 1 << (y % 8);  // Convert Y to page bitmask
  PORTB |= (1 << DC_PIN);            // DC high for data mode
  PORTB &= ~(1 << CS_PIN);           // CS low to enable SPI
  spi_write(page_mask);              // Transmit pixel data
  PORTB |= (1 << CS_PIN);            // CS high to end transaction
}

/**
 * @brief Clears the entire display buffer (sets all pixels to off).
 */
void sh1107_clean() {
  PORTB |= (1 << DC_PIN);   // DC high for data mode
  PORTB &= ~(1 << CS_PIN);  // CS low to enable SPI
  spi_write(0);             // Write 0 to clear pixels
  PORTB |= (1 << CS_PIN);   // CS high to end transaction
}

/**
 * @brief Sets the target page (8-pixel row group) for subsequent writes.
 * @param y The vertical position (0-127) to determine the page.
 */
void sh1107_page(uint8_t y) {
  uint8_t page = y / 8;  // Convert Y coordinate to page number
  sh1107_command(SH1107_SET_PAGE_ADDR + page);
}

/**
 * @brief Sets the lower nibble of the column address for pixel positioning.
 * @param x The horizontal position (0-127), lower 4 bits used.
 */
void sh1107_lowcol(uint8_t x) {
  uint8_t low_nibble = (x & 0x0F);  // Extract lower 4 bits
  sh1107_command(SH1107_SET_LOW_COL_ADDR + low_nibble);
}

/**
 * @brief Sets the higher nibble of the column address for pixel positioning.
 * @param x The horizontal position (0-127), upper 4 bits used.
 */
void sh1107_highcol(uint8_t x) {
  uint8_t high_nibble = ((x & 0xF0) >> 4);  // Extract upper 4 bits
  sh1107_command(SH1107_SET_HIGH_COL_ADDR + high_nibble);
}

/**
 * @brief Initializes the SH1107 display with default settings.
 * Performs hardware reset and configures display parameters:
 * - Clock divider, multiplex ratio, charge pump, addressing mode.
 * - Contrast, precharge, VCOMH, and display state.
 */
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