

sym :- @'PUSH' \any @'Sym' &''.

field_name :- 'FIELD_NAME' sym.

field :-  field_name 'FIELD_TYPE' 'Bool' @'PUSH' @'Int' @'1' @'DEF_FIELD'.
field :-  field_name 'FIELD_TYPE' 'Sym'  @'PUSH' @'Int' @'2' @'DEF_FIELD'.
field :-  field_name 'FIELD_TYPE' 'Int'  @'PUSH' @'Int' @'3' @'DEF_FIELD'.
field :-  field_name 'FIELD_TYPE' 'UInt' @'PUSH' @'Int' @'4' @'DEF_FIELD'.
field :-  field_name 'FIELD_TYPE' 'Real' @'PUSH' @'Int' @'5' @'DEF_FIELD'.
field :-  field_name 'FIELD_TYPE' sym    @'DEF_STRUCT_FIELD'.
field :-  field_name 'EMPTY_TYPE'        @'PUSH' @'Int' @'1' @'DEF_FIELD'.

fields :- field fields.
fields :- .

def :- 'START_DEF' @'_cmode_on' @'NEW_SHAPE' fields 'DEF_NAME' sym 'END_DEF' @'DEF_SHAPE' @'_cmode_off'.



val_literal :- \any &''.

primitive_type :- 'Int' &''.
primitive_type :- 'UInt' &''.
primitive_type :- 'Real' &''.
primitive_type :- 'Bool' &''.
primitive_type :- 'Sym' &''.

primitive_type_x :- primitive_type &'push'.

val :- 'SET_TYPE' @'PUSH' primitive_type_x 'PUSH' val_literal @'_push_type' &'pop'.
val :- 'VOID' @'_push_type' @'Void'.

val_primitive :- val.

structfield :- val_or_call 
               'SELECT_FIELD' @'_fieldname_deref' val_literal
               'CHECK_TYPE' @'_fieldtype_check' val_literal @'_pop_type'
               'SET_FIELD' @'_type_size' @'SET_FIELDS'.

structfields :- structfield structfields.
structfields :- .


structval :- 'SET_TYPE' @'_push_type' val_literal 
             'START_STRUCT' @'_type_size' @'NEW_STRUCT' 
             structfields 
             'END_STRUCT'
             .

change_struct :- 'CHANGE_STRUCT' structfields 'END_CHANGE_STRUCT'.

tuplefields :- val_or_call 'SET_TUPLEFIELD' tuplefields.
tuplefields :- .

tupleval :- 'START_TUPLE' @'_mark_tuple' tuplefields 'END_TUPLE' @'_make_tupletype'.

variable :- 'DEREF' @'_get_fields' val_literal @'GET_FRAMEHEAD_FIELDS'.

funcall :- 'ASMCALL' @'_asmcall' val_literal.

funcall :- 'CALL' @'_call_or_syscall' val_literal val_literal
           @'IF_NOT_FAIL' @'2' @'FAIL'.

funcall :- 'TAILCALL' @'DROP_FRAME' @'_tailcall_or_syscall' val_literal val_literal
           @'IF_NOT_FAIL' @'2' @'FAIL'.

statement :- structval.
statement :- tupleval.
statement :- change_struct.
statement :- variable.
statement :- lambda.
statement :- val_primitive.
statement :- funcall.

statements_x :- statement statements_x.
statements_x :- .

val_or_call :- statement statements_x.


function_fail        :- @'POP_FRAMEHEAD' @'FAIL' @'_pop_funlabel'.

function_succeed     :- @'POP_FRAMEHEAD' @'EXIT' @'_pop_funlabel'.

function_try_next    :- @'IF_FAIL' @'2' @'EXIT' @'POP_FRAMETAIL'.


lambda :- 'START_LAMBDA' @'_push_lambda' val_literal @'CALL'
          lambda_statements_or_branch.

lambda :- 'START_TAIL_LAMBDA' @'DROP_FRAME' @'_push_lambda' val_literal @'TAILCALL'
          lambda_statements_or_branch.

lambda_statements_or_branch :- 'END_LAMBDA' 
                               function_fail 
                               @'IF_NOT_FAIL' @'2' @'FAIL'.

lambda_statements_or_branch :- statement_or_branch lambda_statements_or_branch.

branch :- 'START_BRANCH' @'_push_branch' @'CALL_LIGHT'
          branch_statements_or_branch.

branch_statements_or_branch :- 'END_BRANCH'
                               function_succeed 
                               function_try_next.

branch_statements_or_branch :- statement_or_branch branch_statements_or_branch.


statement_or_branch :- branch.
statement_or_branch :- statement.


fun_statements_or_branch :- 'END_FUN' 
                            function_fail.

fun_statements_or_branch :- statement_or_branch fun_statements_or_branch.

fun :- 'FUN_TYPE' @'_push_funlabel' val_literal val_literal val_literal
       'START_FUN' 
       fun_statements_or_branch.


all :- def all.
all :- fun all.
all :- @'EXIT'.

main :- all.
