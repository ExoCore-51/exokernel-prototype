# =============================================================================
# EXOKERNEL PROTOTYPE - MAKEFILE
# =============================================================================
# 
# Build system for the exokernel kernel core.
# Works with GCC on Windows (MinGW) or Linux.
#
# Usage:
#   make          - Build the kernel demo
#   make test     - Build and run unit tests
#   make clean    - Remove build artifacts
#
# Author: Group 51
# Date: 2026
# =============================================================================

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -I./include -g

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
BUILD_DIR = build

# Source files
SOURCES = $(SRC_DIR)/secure_binding.c \
		  $(SRC_DIR)/access_control.c \
		  $(SRC_DIR)/syscall.c \
		  $(SRC_DIR)/kernel.c \
		  $(SRC_DIR)/memory.c \
		  $(SRC_DIR)/logging.c \
		  $(SRC_DIR)/libos.c

# Main entry point (separate from sources for testing)
MAIN = $(SRC_DIR)/main.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Output executables
KERNEL_EXE = kernel_demo.exe
TEST_EXE = test_kernel.exe

# =============================================================================
# TARGETS
# =============================================================================

# Default target: build the kernel demo
all: $(KERNEL_EXE)
	@echo ""
	@echo "Build complete! Run with: ./$(KERNEL_EXE)"
	@echo ""

# Build the kernel demo executable
$(KERNEL_EXE): $(SOURCES) $(MAIN)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(MAIN)
	@echo "Built $(KERNEL_EXE)"

# Build and run tests
test: $(TEST_EXE)
	@echo ""
	@echo "Running tests..."
	@echo "================"
	./$(TEST_EXE)

# Build the test executable
$(TEST_EXE): $(SOURCES) $(TEST_DIR)/test_kernel.c
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(TEST_DIR)/test_kernel.c
	@echo "Built $(TEST_EXE)"

# Clean build artifacts
clean:
	rm -f $(KERNEL_EXE) $(TEST_EXE)
	rm -f $(SRC_DIR)/*.o
	@echo "Cleaned build artifacts"

# Run the demo
run: $(KERNEL_EXE)
	./$(KERNEL_EXE)

# Phony targets
.PHONY: all test clean run

# =============================================================================
# HELP
# =============================================================================
help:
	@echo ""
	@echo "Exokernel Prototype - Build System"
	@echo "==================================="
	@echo ""
	@echo "Available targets:"
	@echo "  make          - Build the kernel demo"
	@echo "  make test     - Build and run unit tests"
	@echo "  make run      - Run the kernel demo"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make help     - Show this help message"
	@echo ""
