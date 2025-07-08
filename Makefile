SRC = main.c
OUT_DIR = build
TARGET = $(OUT_DIR)/playsprite
CC = gcc
CFLAGS = -Wall -O2

default: build run

build: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -o $@ $<

run:
	./$(TARGET)

clean:
	rm -rf $(OUT_DIR)
