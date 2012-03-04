

sym :- @'PUSH' * @'Sym' &''.

field_name :- 'FIELD_NAME' sym.

field :-  field_name 'FIELD_TYPE' 'Sym'  @'PUSH' @'Int' @'2' @'DEF_FIELD'.
field :-  field_name 'FIELD_TYPE' 'Int'  @'PUSH' @'Int' @'3' @'DEF_FIELD'.
field :-  field_name 'FIELD_TYPE' 'UInt' @'PUSH' @'Int' @'4' @'DEF_FIELD'.
field :-  field_name 'FIELD_TYPE' 'Real' @'PUSH' @'Int' @'5' @'DEF_FIELD'.
field :-  field_name 'FIELD_TYPE' sym    @'DEF_STRUCT_FIELD'.
field :-  field_name 'EMPTY_TYPE'            @'PUSH' @'Int' @'1' @'DEF_FIELD'.

fields :- field fields.
fields :- .

def :- 'START_DEF' @'_cmode_on' @'NEW_SHAPE' fields 'DEF_NAME' sym 'END_DEF' @'DEF_SHAPE' @'_cmode_off'.



val_literal :- * &''.

val :- 'PUSH_INT' @'PUSH' @'Int' val_literal.
val :- 'PUSH_REAL' @'PUSH' @'Real' val_literal.
val :- 'PUSH_SYM' sym.
val :- structval.


structfield :- 'FIND_FIELD_INDEX' @'_fieldname_deref' val_literal val 'SET_FIELD' @'_type_size' @'SET_FIELDS'.

structfields :- structfield structfields.
structfields :- .


structval :- 'SET_TYPE' @'_push_type' val_literal 
             'START_STRUCT' @'_type_size' @'NEW_STRUCT' 
             structfields 
             'END_STRUCT' @'_pop_type'
             .



all :- def all.
all :- structval all.
all :- .

main :- all.
