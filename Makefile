SRC = $(wildcard src/*.c) $(wildcard src/*/*.c)
OBJ = $(SRC:src/%.c=$(OUT_DIR)/obj/%.o)
OUT_DIR = build
TARGET = $(OUT_DIR)/playsprite
CC = gcc
CFLAGS = -Wall -O2

default: build run

build: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OUT_DIR)/obj/%.o: src/%.c | mk
	$(CC) $(CFLAGS) -c $< -o $@

mk:
	mkdir -p $(OUT_DIR)/obj/external

run:
	./$(TARGET)

clean:
	rm -rf $(OUT_DIR)
