#include <avr/io.h>

#include "config.h"

// ======================
// SPI Functions
// ======================
void spi_init() {
  DDRB |=
      (1 << PB3) | (1 << PB5) | (1 << CS_PIN) | (1 << DC_PIN) | (1 << RES_PIN);
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
  PORTB |= (1 << CS_PIN);
}

void spi_write(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)))
    ;
}