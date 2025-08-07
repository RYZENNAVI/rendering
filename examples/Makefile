# Makefile for compiling user_examples with examples-only implementation

CC := gcc
CFLAGS := -Wall -Wextra -g -std=c99 -I../inc -I../include -I. -I..
LDFLAGS := -lm -lpng

# Build directory
BUILD_DIR := build

# Target executable
TARGET := user_examples

# Essential source files from the rendering library (now using original brush.c)
ESSENTIAL_SOURCES := \
	../src/draw.c \
	../src/knots.c \
	../src/list.c \
	../src/transforms.c \
	../src/image.c \
	../src/image_png.c \
	../src/span_list.c \
	../src/rasterization.c \
	../src/color_mixing.c \
	../src/brush.c \
	../src/bezier.c

# Corresponding object files
ESSENTIAL_OBJECTS := $(patsubst ../src/%.c, $(BUILD_DIR)/%.o, $(filter ../src/%.c,$(ESSENTIAL_SOURCES)))

# Main source file
MAIN_SOURCE := user_examples.c
MAIN_OBJECT := $(BUILD_DIR)/user_examples.o

# (Removed helper compilation and brush_fixed patch – original brush.c is used directly)

# All objects
ALL_OBJECTS := $(ESSENTIAL_OBJECTS) $(MAIN_OBJECT)

# Wrapper header directory and file
ROOT_INC_DIR := ../../inc
WRAPPER_HEADER := $(ROOT_INC_DIR)/common.h

# Ensure wrapper header exists before compiling anything
all: $(WRAPPER_HEADER) $(BUILD_DIR) $(TARGET)

$(WRAPPER_HEADER):
	mkdir -p $(ROOT_INC_DIR)
	echo '#include "../sose2025_rendering/inc/common.h"' > $(WRAPPER_HEADER)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p output

# Compile main source
$(MAIN_OBJECT): $(MAIN_SOURCE)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile essential library sources from ../src
$(BUILD_DIR)/%.o: ../src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# (Removed helper compilation and brush_fixed patch – original brush.c is used directly)

# Link executable
$(TARGET): $(ALL_OBJECTS)
	$(CC) $(ALL_OBJECTS) $(LDFLAGS) -o $@

# Test individual scenarios
test-sharp: $(TARGET)
	./$(TARGET) sharp

test-color: $(TARGET)
	./$(TARGET) color

test-overlap: $(TARGET)
	./$(TARGET) overlap

test-relative: $(TARGET)
	./$(TARGET) relative

test-quadratic: $(TARGET)
	./$(TARGET) quadratic

test-transform: $(TARGET)
	./$(TARGET) transform

test-misc: $(TARGET)
	./$(TARGET) misc

test-all: $(TARGET)
	./$(TARGET) all

# Clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
	rm -f output/*.png

# Run tests
test: $(TARGET)
	./$(TARGET) all

.PHONY: all clean test test-sharp test-color test-overlap test-relative test-quadratic test-transform test-misc test-all 