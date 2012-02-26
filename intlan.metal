
:- include 'system.metal'

:- define prelude_compiletime {
   .const number_str 10
   .const opname_str 16
   .const scopes_numbering_start 100
   .const scopes_cell 1

   .strconst "0"    0     100
   .strconst "0f"   0f    101
   .strconst "ADD_"  ADD   102
   .strconst "SUB_"  SUB   103
   .strconst "MUL_"  MUL   104
   .strconst "DIV_"  DIV   105
   .strconst "INT"  INT   106
   .strconst "REAL" REAL  107
   .strconst "INT_TO_REAL\n" INT_TO_REAL  108
   .strconst "CALL(swap)\n" CALL_swap     109

   .label init
   PUSH($scopes_cell)
   SIZE_HEAP(1)
   PUSH($scopes_numbering_start)
   PUSH($scopes_cell)
   TO_HEAP(0)
   RET

   .label sendout
   PUSH($port) FROM_HEAP($out) 
   SYSCALL($str_append)
   RET

   .label concat_number
   PUSH($port) FROM_HEAP($in) 
   PUSH($number_str) 
   SYSCALL($str_append)
   RET

   .label get_number
   PUSH($number_str)
   CALL(sendout)
   PUSH($number_str) 
   SYSCALL($str_free)
   RET

   .label mark_int
   PUSH(0)
   RET

   .label mark_float
   PUSH(1)
   RET

   .label set_opname
   PUSH($opname_str)
   SYSCALL($str_free)
   PUSH($opname_str)
   SYSCALL($str_append)
   RET

   .label int_int_binop
   PUSH($opname_str)
   CALL(sendout)
   PUSH($INT) 
   CALL(sendout)
   PUSH(0)
   RET

   .label real_real_binop
   PUSH($opname_str)
   CALL(sendout)
   PUSH($REAL)
   CALL(sendout)
   PUSH(1)
   RET
   
   .label real_int_binop
   PUSH($INT_TO_REAL)
   CALL(sendout)
   PUSH($opname_str)
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
   PUSH($opname_str)
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

   .label force_int
   POP
   POP
   CALL(mark_int)
   RET

   .label main
   CALL(init)
}

:- define epilogue_compiletime {
   RET
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

:- define epilogue_runtime {
   EXIT
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

neg :- '-' spaces elt 
  @'NEG_' 
  &{ JUMP_CHECK_IF(+3)
       PUSH($INT) JUMP(+2)
       PUSH($REAL) }
  &'CALL(sendout)'
  @'\n'
  .

neg :- elt.

expr_bnot :- '~' spaces neg &{POP CALL(mark_int)} @'BNOT\n'.
expr_bnot :- neg.

expr_bop :- expr_bnot spaces '&' spaces expr_bop &{CALL(force_int)} @'BAND\n'.
expr_bop :- expr_bnot spaces '|' spaces expr_bop &{CALL(force_int)} @'BOR\n'.
expr_bop :- expr_bnot spaces '^' spaces expr_bop &{CALL(force_int)} @'BXOR\n'.
expr_bop :- expr_bnot.

expr_bsh :- expr_bop spaces '>>' spaces int &{CALL(force_int)} @'BSHR\n'.
expr_bsh :- expr_bop spaces '<<' spaces int &{CALL(force_int)} @'BSHL\n'.
expr_bsh :- expr_bop.

expr_m :- expr_bsh spaces * spaces expr_m &{PUSH($MUL) CALL(check_binop)} @'\n'.
expr_m :- expr_bsh spaces / spaces expr_m &{PUSH($DIV) CALL(check_binop)} @'\n'.
expr_m :- expr_bsh.

expr_a :- expr_m spaces + spaces expr_a &{PUSH($ADD) CALL(check_binop)} @'\n'.
expr_a :- expr_m spaces - spaces expr_a &{PUSH($SUB) CALL(check_binop)} @'\n'.
expr_a :- expr_m.

expr :- spaces expr_a spaces.

exprs :- expr ';' exprs.
exprs :- expr.

scope :- spaces 
         '{' &'CALL(start_scope)' 
         exprs 
         '}' 
         spaces.

all :- expr all.
all :- expr.

main :- &prelude_compiletime @prelude_runtime 
        all 
        &epilogue_compiletime @epilogue_runtime.


