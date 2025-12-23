#!/bin/bash
set -e

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Check if code argument is provided
if [ $# -eq 0 ]; then
    echo -e "${RED}Error: No code provided${NC}"
    echo "Usage: cljc \"<code>\""
    echo "Example: cljc \"(+ 1 2 3)\""
    exit 1
fi

CODE="$1"

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Create asm directory if it doesn't exist
mkdir -p asm

# Build the compiler if needed
if [ ! -f build/program ] || [ src/main.c -nt build/program ]; then
    echo -e "${GREEN}Building compiler...${NC}"
    make -s > /dev/null 2>&1
fi

# Run the compiler with the provided code
echo -e "${GREEN}Compiling...${NC}"
./build/program "$CODE" > /dev/null 2>&1

# Check if assembly was generated
if [ ! -f asm/output.s ]; then
    echo -e "${RED}Error: Compilation failed${NC}"
    exit 1
fi

# Build runtime if needed
if [ ! -f build/runtime.o ]; then
    make -s build/runtime.o > /dev/null 2>&1
fi

# Assemble the generated code
as -arch arm64 asm/output.s -o asm/output.o 2>/dev/null

# Link to create the executable
gcc asm/output.o build/runtime.o -o out 2>/dev/null

echo -e "${GREEN}✓ Compiled successfully to ./out${NC}"
