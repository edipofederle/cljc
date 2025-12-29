# Quick Reference - cljc Bootstrap

## ✅ What Works Now

### Strings
```clojure
"hello"                      ; String literal
(str-length "hello")         ; => 5.0
(str-char-at "hello" 1)      ; => 101.0 (ASCII 'e')
(str-concat "hello" "world") ; => "helloworld"
(substring "hello" 0 3)      ; => "hel"
```

### Lists
```clojure
(empty-list)                          ; Create []
(cons 1 (empty-list))                 ; Create [1]
(cons 1 (cons 2 (cons 3 (empty-list)))) ; Create [1, 2, 3]
(first (cons 42 (empty-list)))        ; => 42.0
(rest (cons 1 (cons 2 (empty-list)))) ; => [2]
(append (empty-list) 5)               ; => [5]
(list-count (cons 1 (empty-list)))    ; => 1.0
```

### Math
```clojure
(+ 1 2 3)     ; => 6.0
(- 10 3)      ; => 7.0
(* 2 3 4)     ; => 24.0
(/ 10 2)      ; => 5.0
```

### Comparisons
```clojure
(< 1 2)       ; => 1.0 (true)
(> 5 3)       ; => 1.0 (true)
(= 5 5)       ; => 1.0 (true)
(<= 3 3)      ; => 1.0 (true)
(>= 7 5)      ; => 1.0 (true)
```

### Control Flow
```clojure
(if (> 5 3) 100 200)  ; => 100.0
(if (< 5 3) 100 200)  ; => 200.0
```

### Functions
```clojure
(defn square [x] (* x x))
(square 5)  ; => 25.0

(defn is-positive [n]
  (if (> n 0) 1 0))
(is-positive 5)   ; => 1.0
(is-positive -3)  ; => 0.0
```

## 🎯 Bootstrap POC Example

```clojure
;; Define token creation function
(defn make-token [type value line col]
  (cons type (cons value (cons line (cons col (empty-list))))))

;; Create some tokens
(defn tokenize-simple []
  (append
    (append (empty-list)
      (make-token 0 40 1 1))  ; LEFT_PAREN
    (make-token 5 43 1 2)))   ; SYMBOL

;; Count tokens
(list-count (tokenize-simple))  ; => 2.0 ✅
```

## 📁 Key Files

| File | What It Does |
|------|--------------|
| `BOOTSTRAP_PROGRESS.md` | Full progress report & documentation |
| `QUICK_REFERENCE.md` | This file - quick syntax reference |
| `bootstrap/tokenizer-simple.cljc` | Working POC tokenizer |
| `src/codegen.c` | Code generator (strings & lists added here) |
| `runtime/runtime.c` | Runtime functions (strings & lists) |

## 🚀 Quick Commands

```bash
# Build
make clean && make

# Test string
./cljc '(str-length "hello")'

# Test list
./cljc '(list-count (cons 1 (cons 2 (empty-list))))'

# Test function
./cljc '(defn double [x] (* x 2)) (double 21)'

# Test bootstrap
./cljc '(defn make-token [t v l c] (cons t (cons v (cons l (cons c (empty-list)))))) (first (make-token 0 40 1 1))'
```

## 🔧 Common Patterns

### Building a List
```clojure
;; Manual construction
(cons 1 (cons 2 (cons 3 (empty-list))))

;; With append
(append (append (append (empty-list) 1) 2) 3)
```

### Token Representation
```clojure
;; Token = (type value line column)
(make-token 0 40 1 1)     ; LEFT_PAREN '(' at line 1, col 1
(make-token 4 49 1 2)     ; NUMBER '1' at line 1, col 2
(make-token 5 43 1 3)     ; SYMBOL '+' at line 1, col 3
```

### Character Checking
```clojure
(defn is-digit [c]
  (if (>= c 48)
    (if (<= c 57) 1 0)
    0))

(is-digit 49)  ; => 1.0 (true, '1')
(is-digit 65)  ; => 0.0 (false, 'A')
```

## ⚠️ Current Limitations

- No `let` bindings (use function parameters instead)
- No hash maps (use lists of lists)
- No variadic functions (except built-in operators)
- Only doubles (no integers, booleans, etc.)
- No string literals in token representation (use ASCII codes)
- Minor bugs with nested function calls

## 📚 Full Documentation

See `BOOTSTRAP_PROGRESS.md` for:
- Complete implementation details
- Architecture overview
- What's missing for full self-hosting
- Next steps and roadmap
- Technical notes and design decisions
