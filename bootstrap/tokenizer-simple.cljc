;; Minimal tokenizer POC - tokenizes "(+ 1 2)"
;; Demonstrates self-hosting concept

(defn make-token [type value line col]
  (cons type (cons value (cons line (cons col (empty-list))))))

;; Hardcoded tokenization of "(+ 1 2)" for POC
(defn tokenize-poc []
  (append
    (append
      (append
        (append
          (append
            (append (empty-list)
              (make-token 0 40 1 1))    ;; LEFT_PAREN '('
            (make-token 5 43 1 2))      ;; SYMBOL '+'
          (make-token 4 49 1 4))        ;; NUMBER '1'
        (make-token 4 50 1 6))          ;; NUMBER '2'
      (make-token 1 41 1 7))            ;; RIGHT_PAREN ')'
    (make-token 6 0 1 8)))              ;; EOF

;; Test: count the tokens
(list-count (tokenize-poc))
