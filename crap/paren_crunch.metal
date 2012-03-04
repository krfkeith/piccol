
close_atom_or_paren :- ')'.
close_atom_or_paren :- atom_or_paren close_atom_or_paren.

atom_or_paren :- '(' close_atom_or_paren.
atom_or_paren :- *.

close_double_parens :- ')' ')' @')'.
close_double_parens :- atom_or_paren &'' close_double_parens.

double_parens :- '(' '(' @'(' close_double_parens.

int :- 'integer' ':' * &''.
real :- 'real' ':' * &''.

val_parens :- '(' int ')'.
val_parens :- '(' real ')'.

any :- * &''.

all :- double_parens all.
all :- val_parens all.
all :- any all.
all :- .

main :- all.
