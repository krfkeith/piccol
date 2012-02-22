
space :- ' '.
space :- '\n'.
space :- '\t'.

spaces :- space spaces.
spaces :- .

digit :- 0  @'PUSH(0)'.
digit :- 1  @'PUSH(1)'.
digit :- 2  @'PUSH(2)'.
digit :- 3  @'PUSH(3)'.
digit :- 4  @'PUSH(4)'.
digit :- 5  @'PUSH(5)'.
digit :- 6  @'PUSH(6)'.
digit :- 7  @'PUSH(7)'.
digit :- 8  @'PUSH(8)'.
digit :- 9  @'PUSH(9)'.

int1 :- @'PUSH(10) MUL_INT' digit @'ADD_INT' int1.
int1 :- .

int :- digit int1.
int :- digit.

elt :- ( expr ).
elt :- - @'PUSH(0)' int @'SUB_INT'.
elt :- int.

expr_m :- elt * expr_m @'MUL_INT'.
expr_m :- elt / expr_m @'DIV_INT'.
expr_m :- elt.

expr_a :- expr_m + expr_a @'ADD_INT'.
expr_a :- expr_m - expr_a @'SUB_INT'.
expr_a :- expr_m.

expr :- spaces expr_a spaces @'SYSCALL(1)'.

all :- expr all.
all :- expr.

main :- all.


