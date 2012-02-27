
:- include 'system.metal'

:- define prelude_compiletime {
   .const number_str 10
   .const opname_str 16
   .const tmp_str 15

   .const scopes_numbering_start 100
   .const int_type 0
   .const float_type 1

   .const scopes_cell 1
   .const current_scope_size_cell 2

   .strconst "0"    0     100
   .strconst "0f"   0f    101
   .strconst "ADD_"  ADD   102
   .strconst "SUB_"  SUB   103
   .strconst "MUL_"  MUL   104
   .strconst "DIV_"  DIV   105
   .strconst "INT"  INT   106
   .strconst "REAL" REAL  107
   .strconst "INT_TO_REAL\n" INT_TO_REAL  108
   .strconst "SWAP\n" SWAP 109
   .strconst "scope" SCOPE 110

   .label init
   PUSH($scopes_cell)
   SIZE_HEAP(1)
   PUSH($scopes_numbering_start)
   PUSH($scopes_cell)
   TO_HEAP(0)
   PUSH($current_scope_size_cell)
   SIZE_HEAP(1)
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
   PUSH($int_type)
   RET

   .label mark_float
   PUSH($float_type)
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
   PUSH($int_type)
   RET

   .label real_real_binop
   PUSH($opname_str)
   CALL(sendout)
   PUSH($REAL)
   CALL(sendout)
   PUSH($float_type)
   RET
   
   .label real_int_binop
   PUSH($INT_TO_REAL)
   CALL(sendout)
   PUSH($opname_str)
   CALL(sendout)
   PUSH($REAL)
   CALL(sendout)
   PUSH($float_type)
   RET

   .label int_real_binop
   PUSH($SWAP)
   CALL(sendout)
   PUSH($INT_TO_REAL)
   CALL(sendout)
   PUSH($SWAP)
   CALL(sendout)
   PUSH($opname_str)
   CALL(sendout)
   PUSH($REAL)
   CALL(sendout)
   PUSH($float_type)
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

   .label start_scope
   PUSH($scopes_cell)
   FROM_HEAP(0)
   PUSH(1)
   ADD_UINT
   PUSH($scopes_cell)
   TO_HEAP(0)
   PUSH(0)
   PUSH($current_scope_size_cell)
   TO_HEAP(0)
   RET

   .label get_cur_scopetag
   PUSH($SCOPE)
   CALL(sendout)
   PUSH($scopes_cell)
   FROM_HEAP(0)
   PUSH($tmp_str)
   SYSCALL($uint_to_str)
   PUSH($tmp_str)
   CALL(sendout)
   RET

   .label get_varindex
   PUSH($port) FROM_HEAP($in)
   PUSH($scopes_cell) FROM_HEAP(0)
   SYSCALL($symtab_get)
   PUSH($tmp_str)
   SYSCALL($uint_to_str)
   PUSH($tmp_str)
   CALL(sendout)
   RET

   .label get_var_mark_cell
   PUSH($scopes_numbering_start)
   PUSH($port) FROM_HEAP($in)
   PUSH(0)
   SYSCALL($symtab_get)
   ADD_UINT
   RET

   .label get_var_type
   .comment TODO: check for uninitialized variables!
   CALL(get_var_mark_cell)
   SIZE_HEAP(1)
   CALL(get_var_mark_cell)
   FROM_HEAP(0)
   RET

   .label main
   CALL(init)
}

:- define epilogue_compiletime {
   RET
}

:- define prelude_runtime {

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

var_x :- any_letter.
var_x :- digit.
var_x :- '_'.
var_x :- .

var :- any_letter var_x 
       @'PUSH(' 
       &'CALL(get_cur_scopetag)'
       @') FROM_HEAP('
       &'CALL(get_varindex)'
       @')\n'
       &'CALL(get_var_type)'
       .

elt :- ( spaces expr spaces ).
elt :- real.
elt :- int.
elt :- var.

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

var_lval :- var &'CALL(get_var_mark_cell)'.

statement :- spaces var_lval spaces '=' expr &{SWAP TO_HEAP(0)}.
statement :- expr &{POP}.

statements :- statement ';' statements.
statements :- statement.

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


