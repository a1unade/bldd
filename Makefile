CC = gcc

CFLAGS = -Wall -Wextra -Werror -std=c11
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -Isrc

SRC_DIR = src
BUILD_DIR = build
EXECS_DIR = execs

SOURCES = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/cli/cli.c \
	$(SRC_DIR)/scanner/scanner.c \
	$(SRC_DIR)/elf/elf-parser.c \
	$(SRC_DIR)/matcher/matcher.c \
	$(SRC_DIR)/report/report.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TARGET = bldd

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) -lelf

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

samples:
	@if [ -d "$(EXECS_DIR)" ]; then \
		echo "$(EXECS_DIR) already exists"; \
	else \
		git clone https://github.com/JonathanSalwan/binary-samples.git "$(EXECS_DIR)"; \
	fi

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

clean-samples:
	rm -rf $(EXECS_DIR)

.PHONY: all clean samples clean-samples
