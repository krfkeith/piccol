

sym :- @'PUSH' * &'Sym'.

field_name :- 'FIELD_NAME' ':' sym.

field :-  field_name 'FIELD_TYPE' ':' 'Sym'  @'PUSH' @'Int' @'2' @'ADD_FIELD'.
field :-  field_name 'FIELD_TYPE' ':' 'Int'  @'PUSH' @'Int' @'3' @'ADD_FIELD'.
field :-  field_name 'FIELD_TYPE' ':' 'UInt' @'PUSH' @'Int' @'4' @'ADD_FIELD'.
field :-  field_name 'FIELD_TYPE' ':' 'Real' @'PUSH' @'Int' @'5' @'ADD_FIELD'.
field :-  field_name 'FIELD_TYPE' ':' sym    @'ADD_STRUCT_FIELD'.
field :-  field_name 'EMPTY_TYPE'            @'PUSH' @'Int' @'1' @'ADD_FIELD'.

fields :- field fields.
fields :- .

def :- 'START_DEF' @'NEW_SHAPE' fields 'DEF_NAME' ':' sym 'END_DEF' @'DEF_SHAPE'.

all :- def all.
all :- .

main :- all.
