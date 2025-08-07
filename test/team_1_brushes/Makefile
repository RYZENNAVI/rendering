.PHONY: all clean profile unit

CC := gcc
CFLAGS := -pg -g -Wall -Wextra -std=c99 -pedantic -O0 -I../../inc -I/opt/local/inc
LDFLAGS := -pg -L/opt/local/lib -lm

SRC_DIR := ../../src
TEST_DIR := .

# Source files
BRUSH := $(SRC_DIR)/brush.o
DRAW := $(SRC_DIR)/draw.o
KNOTS := $(SRC_DIR)/knots.o
LIST := $(SRC_DIR)/list.o
BEZIER := $(SRC_DIR)/bezier.o
TRANSFORMS := $(SRC_DIR)/transforms.o

# Executable object groups
BEZIER_OBJS := $(BRUSH) $(DRAW) $(KNOTS) $(LIST) $(TRANSFORMS) $(BEZIER) $(TEST_DIR)/brush_test.o
UNIT_OBJS := $(BRUSH) $(DRAW) $(KNOTS) $(LIST) $(TRANSFORMS) $(BEZIER) $(TEST_DIR)/transforms_test.o

# Default
all: test

# Main test executable
test: $(BEZIER_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Unit tests
unit: $(UNIT_OBJS)
	$(CC) $(CFLAGS) $^ -o unit_tests $(LDFLAGS)
	./unit_tests

# Pattern rules
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Profiling
profile: test
	./test
	gprof test gmon.out > report.txt
	@echo "Profiling complete. See report.txt."

# Clean build
clean:
	rm -vf $(SRC_DIR)/*.o $(TEST_DIR)/*.o test unit_tests gmon.out report.txt