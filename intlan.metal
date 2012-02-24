
:- include 'system.metal'

:- define prelude_compiletime {
   .const(number_sym, 10)
   .const(opname_sym, 16)
   .const(0,    11)
   .const(0f,   12)
   .const(INT,  18)
   .const(SUB,  13)
   .const(MUL,  19)
   .const(DIV,  20)
   .const(INT,  14)
   .const(REAL, 15)
   .const(INT_TO_REAL, 17)
   .symbol("0",    $0)
   .symbol("0f",   $0f)
   .symbol("ADD",  $ADD)
   .symbol("SUB",  $SUB)
   .symbol("MUL",  $MUL)
   .symbol("DIV",  $DIV)
   .symbol("INT",  $INT)
   .symbol("REAL", $REAL)
   .symbol("INT_TO_REAL\n", $INT_TO_REAL)
   .symbol("CALL(swap)\n", $CALL_swap)

   .label(sendout)
   PUSH($port) FROM_HEAP($out) 
   SYSCALL($str_append)
   RET

   .label(int_int_binop)
   PUSH(opname_sym)
   CALL(sendout)
   PUSH($INT) 
   CALL(sendout)
   PUSH(0)
   RET

   .label(real_real_binop)
   PUSH(opname_sym)
   CALL(sendout)
   PUSH($REAL)
   CALL(sendout)
   PUSH(1)
   RET
   
   .label(real_int_binop)
   PUSH($INT_TO_REAL)
   CALL(sendout)
   PUSH(opname_sym)
   CALL(sendout)
   PUSH($REAL)
   CALL(sendout)
   PUSH(1)
   RET

   .label(int_real_binop)
   PUSH($CALL_swap)
   CALL(sendout)
   PUSH($INT_TO_REAL)
   CALL(sendout)
   PUSH($CALL_swap)
   CALL(sendout)
   PUSH($REAL)
   CALL(sendout)
   PUSH(1)
   RET

   .label(x_int_binop)
   JUMP_IF(+3)
   CALL(int_int_binop)
   RET
   CALL(real_int_binop)
   RET

   .label(x_real_binop)
   JUMP_IF(+3)
   CALL(int_real_binop)
   RET
   CALL(real_real_binop)
   RET

   .label(check_binop)
   JUMP_IF(+3)
   CALL(x_int_binop)
   RET
   CALL(x_real_binop)
   RET
}

:- define prelude_runtime {
   .const(swap_cell, 10)
   PUSH($swap_cell) SIZE_HEAP(2)

   .label(swap)
   PUSH($swap_cell) TO_HEAP(0)
   PUSH($swap_cell) TO_HEAP(1)
   PUSH($swap_cell) FROM_HEAP(0)
   PUSH($swap_cell) FROM_HEAP(1)
   RET
}

:- define concat_number {
   PUSH($port) FROM_HEAP($in) 
   PUSH($number_sym) 
   SYSCALL($str_append)
}

:- define get_number {
   PUSH($number_sym) 
   PUSH($port) FROM_HEAP($out) 
   SYSCALL($str_append) 
   PUSH($number_sym) 
   SYSCALL($str_free)
}

:- define mark_int {
   PUSH(0)
}

:- define mark_float {
   PUSH(1)
}

:- define sendout {
   PUSH($port) FROM_HEAP($out) 
   SYSCALL($str_append)
}


int_x :- digit &concat_number int_x.
int_x :- digit &concat_number.

number_dot :- '.' &concat_number.

real_x :- int_x number_dot int_x.

int :- int_x @'PUSH(' &get_number @')\n' &mark_int.

real :- real_x @'PUSH(' &get_number @'f)\n' &mark_float.


elt :- ( spaces expr spaces ).
elt :- real.
elt :- int.

neg :- '-' elt 
  @'NEG_' 
  &{ JUMP_CHECK_IF(+3)
       PUSH($INT) JUMP(+2)
       PUSH($REAL) }
  &sendout
  @'\n'
  .

neg :- elt.

expr_m :- neg spaces * spaces expr_m @'MUL_INT\n'.
expr_m :- neg spaces / spaces expr_m @'DIV_INT\n'.
expr_m :- neg.

expr_a :- expr_m spaces + spaces expr_a @'ADD_INT\n'.
expr_a :- expr_m spaces - spaces expr_a @'SUB_INT\n'.
expr_a :- expr_m.

expr :- spaces expr_a spaces.

all :- expr all.
all :- expr.

main :- &prelude_compiletime @prelude_runtime all.


