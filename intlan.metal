
space :- ' '.
space :- '\n'.
space :- '\t'.

spaces :- space spaces.
spaces :- .

digit :- 0  @'PUSH(0)\n'.
digit :- 1  @'PUSH(1)\n'.
digit :- 2  @'PUSH(2)\n'.
digit :- 3  @'PUSH(3)\n'.
digit :- 4  @'PUSH(4)\n'.
digit :- 5  @'PUSH(5)\n'.
digit :- 6  @'PUSH(6)\n'.
digit :- 7  @'PUSH(7)\n'.
digit :- 8  @'PUSH(8)\n'.
digit :- 9  @'PUSH(9)\n'.

int1 :- @'PUSH(10) MUL_INT\n' digit @'ADD_INT\n' int1.
int1 :- .

int :- digit int1.
int :- digit.

elt :- ( spaces expr spaces ).
elt :- - @'PUSH(0)\n' int @'SUB_INT\n'.
elt :- int.

expr_m :- elt spaces * spaces expr_m @'MUL_INT\n'.
expr_m :- elt spaces / spaces expr_m @'DIV_INT\n'.
expr_m :- elt.

expr_a :- expr_m spaces + spaces expr_a @'ADD_INT\n'.
expr_a :- expr_m spaces - spaces expr_a @'SUB_INT\n'.
expr_a :- expr_m.

expr :- spaces expr_a spaces @'SYSCALL(1)\n'.

all :- expr all.
all :- expr.

main :- all.


