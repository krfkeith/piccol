

real :- 'real' ':' * &''.
int :- 'integer' ':' * &''.

paren :- '(' int ')'.
paren :- '(' real ')'.

parens :- '(' parens ')'.
parens :- paren.

any :- * &''.

all :- parens all.
all :- any all.
all :-.

main :- all.
