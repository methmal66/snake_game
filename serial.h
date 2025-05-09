/**
 * @file serial.h
 * @brief Header file for SPI communication with SH1107 OLED display.
 */

#ifndef SNAKE_GAME_SERIAL_H
#define SNAKE_GAME_SERIAL_H

#include <stdint.h>

void spi_init();

void spi_write(uint8_t);

#endif