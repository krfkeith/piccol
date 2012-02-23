
space :- ' '.
space :- '\n'.
space :- '\t'.

spaces :- space spaces.
spaces :- .

digit :- 0.
digit :- 1.
digit :- 2.
digit :- 3.
digit :- 4.
digit :- 5.
digit :- 6.
digit :- 7.
digit :- 8.
digit :- 9.

digit_x :- digit &'PUSH(10) PUSH($input) FROM_HEAP(0) SYSCALL($str_append)'.

int_x :- digit_x int_x.
int_x :- digit_x.

int :- int_x @'PUSH(' &'PUSH($output) FROM_HEAP(0) PUSH(10) SYSCALL($str_append) PUSH(10) SYSCALL($str_free)' @')\n'.

elt :- ( spaces expr spaces ).
elt :- - @'PUSH(0)\n' int @'SUB_INT\n'.
elt :- int.

expr_m :- elt spaces * spaces expr_m @'MUL_INT\n'.
expr_m :- elt spaces / spaces expr_m @'DIV_INT\n'.
expr_m :- elt.

expr_a :- expr_m spaces + spaces expr_a @'ADD_INT\n'.
expr_a :- expr_m spaces - spaces expr_a @'SUB_INT\n'.
expr_a :- expr_m.

expr :- spaces expr_a spaces.

all :- expr all.
all :- expr.

main :- all.


