# Mini Clojure Compiler

A VERY TINY compiler for a VERY minimal Clojure-like language that generates native ARM64 assembly code for macOS Apple Silicon.

## Features

- **Arithmetic Operations**: `+`, `-`, `*`, `/` with support for multiple arguments
- **Basic def**: `(def foo 10) (+ foo 10)`
- **Nested Expressions**: Unlimited nesting of function calls and operations
- **Function Definitions**: Define functions `defn`
- **Function Calls**: Call user-defined functions
- **Native Compilation**: Generates ARM64 assembly code that runs directly on Apple Silicon Macs
