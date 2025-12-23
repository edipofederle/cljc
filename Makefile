CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I./include
LDFLAGS =
TARGET = program
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests
ASM_DIR = asm
RUNTIME_DIR = runtime

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ASM_DIR):
	mkdir -p $(ASM_DIR)

# Compile runtime
$(BUILD_DIR)/runtime.o: $(RUNTIME_DIR)/runtime.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run compiler to generate assembly
compile: $(BUILD_DIR)/$(TARGET) | $(ASM_DIR)
	./$(BUILD_DIR)/$(TARGET)

# Assemble the generated .s file
$(ASM_DIR)/output.o: $(ASM_DIR)/output.s
	as -arch arm64 $(ASM_DIR)/output.s -o $(ASM_DIR)/output.o

# Link to create final executable
$(ASM_DIR)/program: $(ASM_DIR)/output.o $(BUILD_DIR)/runtime.o
	$(CC) $(ASM_DIR)/output.o $(BUILD_DIR)/runtime.o -o $(ASM_DIR)/program

# Full compilation pipeline
asm-compile: compile $(ASM_DIR)/output.o $(BUILD_DIR)/runtime.o $(ASM_DIR)/program

# Run the compiled assembly program
asm-run: asm-compile
	./$(ASM_DIR)/program

clean:
	rm -rf $(BUILD_DIR) $(ASM_DIR)

run: $(BUILD_DIR)/$(TARGET)
	./$(BUILD_DIR)/$(TARGET)

.PHONY: all clean run compile asm-compile asm-run
