# Default compiler
CC = gcc
CFLAGS = -Wall -I./src/inc
LDFLAGS =

# Cross-compiler
CROSS_CC = aarch64-linux-gnu-gcc
CROSS_CFLAGS = -Wall -I./src/inc
CROSS_LDFLAGS = -static

# Define source directories
SRC_DIR_CORE = src/core
SRC_DIR_INC = src/inc
BUILD_DIR = build
CROSS_BUILD_DIR = cross_build

# Automatically find source files
SRCS = $(wildcard $(SRC_DIR_CORE)/*.c)
OBJS = $(patsubst $(SRC_DIR_CORE)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
CROSS_OBJS = $(patsubst $(SRC_DIR_CORE)/%.c,$(CROSS_BUILD_DIR)/%.o,$(SRCS))

# Define targets
TARGET = spi_program
TARGET_PROGRAM = $(BUILD_DIR)/${TARGET}
CROSS_TARGET_PROGRAM = $(CROSS_BUILD_DIR)/${TARGET}

# Default target
all: $(TARGET_PROGRAM) $(CROSS_TARGET_PROGRAM)

# Build combined program
$(TARGET_PROGRAM): $(OBJS)
	@echo "Building default combined program..."
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

# Cross-compile combined program
$(CROSS_TARGET_PROGRAM): $(CROSS_OBJS)
	@echo "Building cross-compiled combined program..."
	$(CROSS_CC) $(CROSS_CFLAGS) $(CROSS_LDFLAGS) -o $@ $(CROSS_OBJS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR_CORE)/%.c
	mkdir -p $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile source files to object files for cross-compilation
$(CROSS_BUILD_DIR)/%.o: $(SRC_DIR_CORE)/%.c
	mkdir -p $(CROSS_BUILD_DIR)
	@echo "Cross-compiling $<..."
	$(CROSS_CC) $(CROSS_CFLAGS) -c -o $@ $<

# Clean up build files
clean:
	@echo "Cleaning build directories..."
	rm -rf $(BUILD_DIR)
	rm -rf $(CROSS_BUILD_DIR)

# Phony targets
.PHONY: all clean