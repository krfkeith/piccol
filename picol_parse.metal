

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

def :- 'START_DEF' @'NEW_SHAPE' fields 'DEF_NAME' sym 'END_DEF' @'DEF_SHAPE'.



structfield :- 'FIND_FIELD_INDEX' @'_fieldname_deref' val_literal val 'SET_FIELD' @'ADD_FIELD'.

structfields :- structfield structfields.
structfields :- .

val_literal :- * &''.

val :- 'PUSH_INT' @'PUSH' @'Int' val_literal.
val :- 'PUSH_REAL' @'PUSH' @'Real' val_literal.
val :- 'PUSH_SYM' sym.
val :- 'SET_TYPE' @'_push_type' val_literal 'START_STRUCT' @'NEW_STRUCT' structfields 'END_STRUCT' @'DEF_STRUCT'.

all :- def all.
all :- val all.
all :- .

main :- all.
