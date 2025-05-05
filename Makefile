CC = avr-gcc 
SPEED = 16000000UL
MCU = atmega328p
PORT = /dev/cu.usbmodem140111
BIN_DIR = ./bin
BOARD = arduino:avr:uno
CLI = arduino-cli
SKETCH = 7segment.ino

default:
	make build
	make upload 
	make clean

build: main.c config.h
	mkdir -p $(BIN_DIR)
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=$(MCU) -c -o $(BIN_DIR)/main.o main.c
	avr-gcc -mmcu=$(MCU) -o $(BIN_DIR)/main.bin $(BIN_DIR)/main.o
	avr-objcopy -O ihex -R .eeprom $(BIN_DIR)/main.bin $(BIN_DIR)/main.hex

upload: $(BIN_DIR)/main.hex
	avrdude -F -V -c arduino -p $(MCU) -P $(PORT) -b 115200 -U flash:w:$(BIN_DIR)/main.hex

clean:
	rm -rf $(BIN_DIR)