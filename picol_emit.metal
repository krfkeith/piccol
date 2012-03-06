

sym :- @'PUSH' * @'Sym' &''.

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



val_literal :- * &''.

val :- 'SET_TYPE' 'Int'  'PUSH' @'PUSH' @'Int'  val_literal.
val :- 'SET_TYPE' 'Real' 'PUSH' @'PUSH' @'Real' val_literal.
val :- 'SET_TYPE' 'Bool' 'PUSH' @'PUSH' @'Bool' val_literal.
val :- 'SET_TYPE' 'Sym'  'PUSH' sym.
val :- structval.

structfield :- val 'SELECT_FIELD' @'_fieldname_deref' val_literal
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
