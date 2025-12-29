;; Simple tokenizer written in cljc
;; Tokens are represented as lists: (type value line col)
;; Token types: 0=LPAREN, 1=RPAREN, 4=NUMBER, 5=SYMBOL

(defn is-digit [c]
  (if (>= c 48)
    (if (<= c 57) 1 0)
    0))

(defn is-space [c]
  (if (= c 32) 1
    (if (= c 9) 1
      (if (= c 10) 1
        (if (= c 13) 1 0)))))

(defn skip-whitespace [input pos len]
  (if (>= pos len) pos
    (if (is-space (str-char-at input pos))
      (skip-whitespace input (+ pos 1) len)
      pos)))

(defn make-token [type value line col]
  (cons type (cons value (cons line (cons col (empty-list))))))

(defn tokenize-number [input pos len line col start-pos]
  (if (>= pos len)
    (cons (make-token 4 (str-char-at input start-pos) line col) (cons pos (empty-list)))
    (if (is-digit (str-char-at input pos))
      (tokenize-number input (+ pos 1) len line col start-pos)
      (cons (make-token 4 (str-char-at input start-pos) line col) (cons pos (empty-list))))))

(defn tokenize-simple [input pos len line col tokens]
  (if (>= pos len)
    tokens
    (if (is-space (str-char-at input pos))
      (tokenize-simple input (+ pos 1) len line (+ col 1) tokens)
      (if (= (str-char-at input pos) 40)
        (tokenize-simple input (+ pos 1) len line (+ col 1)
          (append tokens (make-token 0 40 line col)))
        (if (= (str-char-at input pos) 41)
          (tokenize-simple input (+ pos 1) len line (+ col 1)
            (append tokens (make-token 1 41 line col)))
          (if (is-digit (str-char-at input pos))
            (tokenize-simple input (+ pos 1) len line (+ col 1)
              (append tokens (make-token 4 (str-char-at input pos) line col)))
            (tokenize-simple input (+ pos 1) len line (+ col 1)
              (append tokens (make-token 5 (str-char-at input pos) line col)))))))))

(defn tokenize [input]
  (tokenize-simple input 0 (str-length input) 1 1 (empty-list)))

;; Test: tokenize a simple expression
(list-count (tokenize "(+ 1 2)"))
