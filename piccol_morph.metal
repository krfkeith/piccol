
:- comment {

   This code inserts a tag to check that field types match:

   SET_TYPE <type> <val> SELECT_FIELD <name> SET_FIELD
    ==>
   SET_TYPE <type> <val> SELECT_FIELD <name> CHECK_TYPE <name> <type> SET_FIELD


   SET_TYPE <type1> <val> CALL <type2> 
    ==>
   use <type2> instead of <type1>, transform "CALL <type2>" to "CALL <type1> <type2>".
}.



typetag_push :- 'PUSH' \* &''.


typetag_structfields :- typetag_field typetag_structfields.
typetag_structfields :- .

typetag_struct :- 'START_STRUCT' @'START_STRUCT' typetag_structfields 'END_STRUCT' @'END_STRUCT'.


typetag_val :- typetag_push.
typetag_val :- typetag_struct.

typetag_name_x :- \* &'' &'push'.

typetag_type :- 'SET_TYPE' @'SET_TYPE' typetag_name_x.

typetag_opt_call :- 'CALL' @'CALL' &'pop' typetag_name_x.
typetag_opt_call :- .

typetag_set_field :- 'SELECT_FIELD' @'SELECT_FIELD'
                     typetag_name_x 
                     @'CHECK_TYPE' &'pop' &'pop'
                     'SET_FIELD' @'SET_FIELD'.

typetag_field :- typetag_type 
                 typetag_val typetag_opt_call 
                 typetag_set_field.

all :- typetag_field all.
all :- \* &'' all.
all :- .

main :- all.

