/**
 * @file serial.c
 * @brief SPI (Serial Peripheral Interface) driver for AVR microcontrollers.
 */

#include <avr/io.h>
#include "config.h"

/**
 * @brief Initializes the SPI interface in master mode.
 * Configures:
 * - MOSI (PB3), SCK (PB5), CS_PIN, DC_PIN, and RES_PIN as outputs.
 * - SPI control register (SPCR) for master mode, clock speed (fosc/16).
 * - Sets CS_PIN high (inactive) by default.
 */
void spi_init() {
  DDRB |=
      (1 << PB3) | (1 << PB5) | (1 << CS_PIN) | (1 << DC_PIN) | (1 << RES_PIN);
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
  PORTB |= (1 << CS_PIN);
}

/**
 * @brief Transmits a single byte over SPI.
 * @param data The byte to transmit.
 * @note Blocks until transmission completes (SPI busy-wait).
 */
void spi_write(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)))
    ;
}