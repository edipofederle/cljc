#!/bin/bash

# cljc - Wrapper script for the cljc compiler
# Usage: ./cljc '(+ 1 2 3)'

if [ $# -lt 1 ]; then
    echo "Usage: $0 \"<clojure code>\""
    echo "Example: $0 \"(+ 1 2 3)\""
    exit 1
fi

# Ensure directories exist
mkdir -p asm build

# Compile to assembly
./build/program "$1" > /dev/null 2>&1 || {
    echo "Compilation failed!"
    ./build/program "$1"
    exit 1
}

# Assemble
as -arch arm64 asm/output.s -o asm/output.o 2>/dev/null || {
    echo "Assembly failed!"
    exit 1
}

# Compile runtime if needed
if [ ! -f build/runtime.o ] || [ runtime/runtime.c -nt build/runtime.o ]; then
    gcc -Wall -Wextra -std=c11 -I./include -c runtime/runtime.c -o build/runtime.o
fi

# Link
gcc asm/output.o build/runtime.o -o asm/program 2>/dev/null || {
    echo "Linking failed!"
    exit 1
}

# Run and capture output
./asm/program
