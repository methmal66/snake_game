CC = avr-gcc 
SPEED = 16000000UL
MCU = atmega328p
PORT = /dev/cu.usbmodem140111
BIN_DIR = ./bin
BOARD = arduino:avr:uno
CLI = arduino-cli
SKETCH = 7segment.ino
COMPILER_FLAGS = -DF_CPU=$(SPEED) -mmcu=$(MCU)

# Source files
SRCS = main.c serial.c display.c graphic.c game.c
OBJS = $(addprefix $(BIN_DIR)/, $(SRCS:.c=.o))
HEADERS = config.h font.h serial.h display.h graphic.h game.h

default: build upload clean

build: $(HEADERS) $(SRCS)
	mkdir -p $(BIN_DIR)
	# Compile each source file
	@for src in $(SRCS); do \
		echo "Compiling $$src..."; \
		$(CC) -Os $(COMPILER_FLAGS) -c -o $(BIN_DIR)/$${src%.c}.o $$src; \
	done
	
	# Link objects into binary
	$(CC) -mmcu=$(MCU) -o $(BIN_DIR)/main.bin $(OBJS)
	avr-objcopy -O ihex -R .eeprom $(BIN_DIR)/main.bin $(BIN_DIR)/main.hex

upload: $(BIN_DIR)/main.hex
	avrdude -F -V -c arduino -p $(MCU) -P $(PORT) -b 115200 -U flash:w:$(BIN_DIR)/main.hex

clean:
	rm -rf $(BIN_DIR)