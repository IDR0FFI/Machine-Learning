CC = gcc
CFLAGS = -Wall -Wextra -g -I./src
LDLIBS = -lm

BUILD_DIR = ./build

OBJS = \
			 $(BUILD_DIR)/main.o \
			 $(BUILD_DIR)/dataset.o \
			 $(BUILD_DIR)/util.o \
			 $(BUILD_DIR)/model.o

HEADER = \
				./src/dataset_config.h \
				./src/dataset.h \
				./src/util.h \
				./src/model.h

all: $(BUILD_DIR)/main

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: ./src/%.c $(HEADER) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

run:
	$(BUILD_DIR)/main

clean:
	rm -rf ./build
.PHONY: all run clean
