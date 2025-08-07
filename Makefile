CC := clang  # Compiler

SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := inc
PROGRAM_NAME := curves 

CFLAGS := -I $(INCLUDE_DIR) -lm -lpng # Compiler flags, include math library. -I allowes us to specify the location of the header files (cf. man gcc)

# Source files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)  # to get all *.c files

# Object files
BUILD := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))  # Performs a textual replacement on the text text (cf. GNU)

all: $(PROGRAM_NAME)				
	@echo "Object files:"
	@echo $(BUILD)

# every .o is created from .c files with the same name 
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $< to $@"
	$(CC) $(CFLAGS) -c $< -o $@

# link object files into an executable
$(PROGRAM_NAME): $(BUILD)
	@echo "Linking object files into executable: $@"
	$(CC) $(BUILD) $(CFLAGS) -o $(PROGRAM_NAME)

test : all
	./curves

# Clean target to remove object files
clean:
	rm -vf $(BUILD_DIR)/*.o $(PROGRAM_NAME)
