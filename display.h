#ifndef SNAKE_GAME_DISPLAY_H
#define SNAKE_GAME_DISPLAY_H

#include <stdint.h>

// Display ON/OFF
#define SH1107_DISPLAY_OFF 0xAE
#define SH1107_DISPLAY_ON 0xAF

// Timing & Clock
#define SH1107_SET_CLOCK_DIV 0xD5
#define SH1107_CLOCK_DIV_DEFAULT 0x80

// Multiplexing
#define SH1107_SET_MULTIPLEX_RATIO 0xA8
#define SH1107_MULTIPLEX_128 0x7F  // For 128x64 displays

// Display Offset
#define SH1107_SET_DISPLAY_OFFSET 0xD3
#define SH1107_OFFSET_NONE 0x00

// Start Line
#define SH1107_SET_START_LINE 0x40  // Default (line 0)

// Charge Pump
#define SH1107_CHARGE_PUMP_CTRL 0x8D
#define SH1107_CHARGE_PUMP_ENABLE 0x14

// Addressing Mode
#define SH1107_SET_ADDRESS_MODE 0x20
#define SH1107_ADDRESS_MODE_HORIZ 0x00

// Segment Remap (X-axis flip)
#define SH1107_SET_SEGMENT_REMAP 0xA1  // 0xA0 = normal, 0xA1 = flipped

// COM Scan Direction (Y-axis flip)
#define SH1107_SET_COM_SCAN_DIR 0xC8  // 0xC0 = normal, 0xC8 = flipped

// COM Pin Config
#define SH1107_SET_COM_PINS 0xDA
#define SH1107_COM_PINS_ALT 0x12  // Alternative pin mapping

// Contrast
#define SH1107_SET_CONTRAST 0x81
#define SH1107_CONTRAST_DEFAULT 0xCF  // Adjust as needed (0-255)

// Pre-charge Period
#define SH1107_SET_PRECHARGE 0xD9
#define SH1107_PRECHARGE_DEFAULT 0xF1  // Phase 1=15, Phase 2=1

// VCOMH Deselect Level
#define SH1107_SET_VCOMH_DESELECT 0xDB
#define SH1107_VCOMH_DEFAULT 0x40  // ~0.77×VCC

// Display Mode
#define SH1107_SET_ENTIRE_DISPLAY 0xA4  // Pixels follow RAM
#define SH1107_SET_NORMAL_DISPLAY 0xA6  // Non-inverted (0xA7 = inverted)

void sh1107_command(uint8_t);

void sh1107_data(uint8_t);

void sh1107_init();

#endif