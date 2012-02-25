
:- include 'system.metal'

:- define prelude_compiletime {
   .const number_sym 10
   .const opname_sym 16

   .symconst "0"    0     100
   .symconst "0f"   0f    101
   .symconst "ADD"  ADD   102
   .symconst "SUB"  SUB   103
   .symconst "MUL"  MUL   104
   .symconst "DIV"  DIV   105
   .symconst "INT"  INT   106
   .symconst "REAL" REAL  107
   .symconst "INT_TO_REAL\n" INT_TO_REAL  108
   .symconst "CALL(swap)\n" CALL_swap     109


   .label sendout
   PUSH($port) FROM_HEAP($out) 
   SYSCALL($str_append)
   RET

   .label concat_number
   PUSH($port) FROM_HEAP($in) 
   PUSH($number_sym) 
   SYSCALL($str_append)
   RET

   .label get_number
   PUSH($number_sym)
   CALL(sendout)
   PUSH($number_sym) 
   SYSCALL($str_free)
   RET

   .label mark_int
   PUSH(0)
   RET

   .label mark_float
   PUSH(1)
   RET

   .label set_opname
   PUSH($opname_sym)
   SYSCALL($str_free)
   PUSH($opname_sym)
   SYSCALL($str_append)
   RET

   .label int_int_binop
   PUSH($opname_sym)
   CALL(sendout)
   PUSH($INT) 
   CALL(sendout)
   PUSH(0)
   RET

   .label real_real_binop
   PUSH($opname_sym)
   CALL(sendout)
   PUSH($REAL)
   CALL(sendout)
   PUSH(1)
   RET
   
   .label real_int_binop
   PUSH($INT_TO_REAL)
   CALL(sendout)
   PUSH($opname_sym)
   CALL(sendout)
   PUSH($REAL)
   CALL(sendout)
   PUSH(1)
   RET

   .label int_real_binop
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

   .label x_int_binop
   JUMP_IF(+3)
   CALL(int_int_binop)
   RET
   CALL(real_int_binop)
   RET

   .label x_real_binop
   JUMP_IF(+3)
   CALL(int_real_binop)
   RET
   CALL(real_real_binop)
   RET

   .label check_binop
   CALL(set_opname)
   JUMP_IF(+3)
   CALL(x_int_binop)
   RET
   CALL(x_real_binop)
   RET

   .label main
}

:- define prelude_runtime {
   .const swap_cell 10
   PUSH($swap_cell) SIZE_HEAP(2)

   .label swap
   PUSH($swap_cell) TO_HEAP(0)
   PUSH($swap_cell) TO_HEAP(1)
   PUSH($swap_cell) FROM_HEAP(0)
   PUSH($swap_cell) FROM_HEAP(1)
   RET

   .label main
}



int_x :- digit &'CALL(concat_number)' int_x.
int_x :- digit &'CALL(concat_number)'.

number_dot :- '.' &'CALL(concat_number)'.

real_x :- int_x number_dot int_x.

int :- int_x @'PUSH(' &'CALL(get_number)' @')\n' &'CALL(mark_int)'.

real :- real_x @'PUSH(' &'CALL(get_number)' @'f)\n' &'CALL(mark_float)'.


elt :- ( spaces expr spaces ).
elt :- real.
elt :- int.

neg :- '-' elt 
  @'NEG_' 
  &{ JUMP_CHECK_IF(+3)
       PUSH($INT) JUMP(+2)
       PUSH($REAL) }
  &'CALL(sendout)'
  @'\n'
  .

neg :- elt.

expr_m :- neg spaces * spaces expr_m &{PUSH($MUL) CALL(check_binop)} @'\n'.
expr_m :- neg spaces / spaces expr_m &{PUSH($DIV) CALL(check_binop)} @'\n'.
expr_m :- neg.

expr_a :- expr_m spaces + spaces expr_a &{PUSH($ADD) CALL(check_binop)} @'\n'.
expr_a :- expr_m spaces - spaces expr_a &{PUSH($SUB) CALL(check_binop)} @'\n'.
expr_a :- expr_m.

expr :- spaces expr_a spaces.

all :- expr all.
all :- expr.

main :- &prelude_compiletime @prelude_runtime all.


