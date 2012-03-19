
(define-generic-mode piccol-mode
  '("#" ("/*" . "*/"))
  '("def" "false" "true")
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
    ("\\W\\(\\$[a-z][a-zA-Z_0-9]*\\)" 1 font-lock-warning-face) ;font-lock-preprocessor-face)
    ("\\W\\([A-Z][a-zA-Z_0-9]*\\)" 1 font-lock-type-face)
    ("\\W\\(\\\\[a-z][a-zA-Z_0-9]*\\)" 1 font-lock-variable-name-face))
  '("\\.lol$" "\\.piccol$")
  nil
  "A mode for simple piccol syntax highlighting.")












