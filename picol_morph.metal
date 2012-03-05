

val :- 'PUSH_INT' * &''.
val :- 'PUSH_REAL' * &''.
val :- 'PUSH_SYM' * &''.
val :- structval.

structval :- 'SET_TYPE' * 'START_STRUCT' &'' structfields 'END_STRUCT' @'END_STRUCT'.

structfields :- swap_struct_field structfields.
structfields :- .

fieldname :- * &'push'.

swap_struct_field :- 'SELECT_FIELD' fieldname val 'SET_FIELD'
                     @'SELECT_FIELD' &'pop' @'SET_FIELD'
                     .

all :- swap_struct_field all.
all :- * &'' all.
all :- .

main :- all.

