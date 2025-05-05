#ifndef SNAKE_GAME_SERIAL_H
#define SNAKE_GAME_SERIAL_H

#include <stdint.h>

void spi_init();

void spi_write(uint8_t d);

#endif
