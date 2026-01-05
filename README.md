# Mini Clojure Compiler

A VERY TINY compiler for a VERY minimal Clojure-like language that generates native ARM64 assembly code for macOS Apple Silicon.

```
(defn factorial [n]
 (if (<= n 1)
      1
      (let [prev (factorial (- n 1))]
        (* n prev))))
(factorial 5)  ; => 120.0
```

## Features

### Core Language
- **Arithmetic Operations**: `+`, `-`, `*`, `/` with support for multiple arguments
- **Comparisons**: `<`, `>`, `=`, `<=`, `>=`
- **Basic def**: `(def foo 10) (+ foo 10)`
- **if expressions**: `(if (> 10 2) 0 1)`
- **let bindings**: `(let [x 5 y 10] (+ x y))` with support for nesting
- **Nested Expressions**: Unlimited nesting of function calls and operations
- **Function Definitions**: `(defn square [x] (* x x))`
- **Function Calls**: Call user-defined functions
- **Native Compilation**: Generates ARM64 assembly code that runs directly on Apple Silicon Macs

### NEW: String Support
- **String Literals**: `"hello"`
- **String Functions**: `str-length`, `str-char-at`, `str-concat`, `substring`
- Example: `(str-length "hello")` → `5.0`

### NEW: List Operations
- **Runtime Lists**: `empty-list`, `cons`, `first`, `rest`, `append`, `list-count`
- Example: `(list-count (cons 1 (cons 2 (empty-list))))` → `2.0`
