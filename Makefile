CC = gcc

CFLAGS = -Wall -Wextra -Werror -std=c11
CPPFLAGS = -Isrc

SRC_DIR = src
BUILD_DIR = build

SOURCES = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/cli/cli.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET = bldd

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
