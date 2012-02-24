
:- include 'system.metal'

:- define concat_number '
   PUSH(10) 
   PUSH($port) 
   FROM_HEAP($in) 
   SYSCALL($str_append)'

:- define get_number '
   PUSH($port) 
   FROM_HEAP($out) 
   PUSH(10) 
   SYSCALL($str_append) 
   PUSH(10) 
   SYSCALL($str_free)'

:- define mark_int '
   PUSH(0)'

:- define mark_float '
   PUSH(1)'

int_x :- digit &concat_number int_x.
int_x :- digit &concat_number.

number_dot :- '.' &concat_number.

real_x :- int_x number_dot int_x.

int :- int_x @'PUSH(' &get_number @')\n' &mark_int.

real :- real_x @'PUSH(' &get_number @'f)\n' &mark_float.





elt :- ( spaces expr spaces ).
elt :- - @'PUSH(0)\n' int @'SUB_INT\n'.
elt :- real.
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


