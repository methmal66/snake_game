#ifndef SNAKE_GAME_DISPLAY_H
#define SNAKE_GAME_DISPLAY_H

#include <stdint.h>

void sh1107_command(uint8_t);

void sh1107_data(uint8_t);

void sh1107_init();

#endif