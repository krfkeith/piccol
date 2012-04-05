
(defvar piccol-macro-face 'piccol-macro-face "piccol-mode face for highlighting macro tags.")
(copy-face 'font-lock-variable-name-face 'piccol-macro-face)
(make-face-bold 'piccol-macro-face)


(define-generic-mode piccol-mode
  '("#" ("/*" . "*/"))
  '("def" "false" "true" "fail")
  '(("'.*?[^\\\\]'" 0 font-lock-string-face t)
    ":-"
    "->"
    "\\[" "\\]"
    "{" "}"
    "(" ")"
    ";"
    "="
    "\\."
    "\\?"
    ("<::" . piccol-macro-face)
    ("::>" . piccol-macro-face)
    ("<:" . piccol-macro-face)
    (":>" . piccol-macro-face)
    ("\\W\\(\\$[a-z][a-zA-Z_0-9]*\\)" 1 font-lock-warning-face) ;font-lock-preprocessor-face)
    ("\\W\\([A-Z][a-zA-Z_0-9]*\\)" 1 font-lock-type-face)
    ("\\W\\(\\\\[a-z][a-zA-Z_0-9]*\\)" 1 font-lock-variable-name-face)
    ("\\W\\\\\\\\\\W" . font-lock-variable-name-face))
  '("\\.lol$" "\\.piccol$")
  nil
  "A mode for simple piccol syntax highlighting.")
