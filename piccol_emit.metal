

sym :- @'PUSH' \any @'Sym' &''.

field_name :- 'FIELD_NAME' sym.

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
primitive_type :- 'Real' &''.
primitive_type :- 'Bool' &''.
primitive_type :- 'Sym' &''.

val :- 'SET_TYPE' @'PUSH' primitive_type 'PUSH' val_literal.
val :- structval.

val_or_call :- val 'CALL' 
               @'PUSH' @'Sym' primitive_type 
               @'PUSH' @'Sym' primitive_type @'SYSCALL_PRIMITIVE'.

val_or_vall :- val 'CALL' sym sym @'CALL'.
val_or_call :- val.

structfield :- val_or_call 
               'SELECT_FIELD' @'_fieldname_deref' val_literal
               'CHECK_TYPE' @'_fieldtype_check' val_literal val_literal
               'SET_FIELD' @'_type_size' @'SET_FIELDS'.

structfields :- structfield structfields.
structfields :- .


structval_head :- 'SET_TYPE' @'_push_type' val_literal 
                  'START_STRUCT' @'_type_size' @'NEW_STRUCT' 
                  structfields 
                  'END_STRUCT'
                  .

structval :- structval_head @'_pop_type'.

structval_toplevel :- structval_head 'SYSCALL' @'_top_type' @'SYSCALL_STRUCT' @'_pop_type'.


all :- def all.
all :- structval_toplevel all.
all :- @'EXIT'.

main :- all.
