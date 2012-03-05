
:- comment {

   Does two things:

   a) Swap arguments around '=' to make it match postfix notation:

   SET_TYPE <type> START_STRUCT ... SELECT_FIELD <name> <val> SET_FIELD ... END_STRUCT 
    ==>
   SET_TYPE <type> START_STRUCT ... <val> SELECT_FIELD <name> SET_FIELD ... END_STRUCT

   b) Insert a tag to check that field types match:

   PUSH_[INT|REAL|SYM|BOOL] <val> SELECT_FIELD <name> SET_FIELD
    ==>
   PUSH_[INT|REAL|SYM|BOOL] <val> SELECT_FIELD <name> CHECK_TYPE <name> [Int|Real|Sym|Bool] SET_FIELD

   SET_TYPE <type> <struct> SELECT_FIELD <name> SET_FIELD
    ==>
   SET_TYPE <type> <struct> SELECT_FIELD <name> CHECK_TYPE <name> <type> SET_FIELD

}.

swap_val :- 'PUSH_INT'  * &''.
swap_val :- 'PUSH_REAL' * &''.
swap_val :- 'PUSH_SYM'  * &''.
swap_val :- 'PUSH_BOOL' * &''.
swap_val :- swap_structval.

swap_structval :- 'SET_TYPE' * 'START_STRUCT' &'' swap_structfields 'END_STRUCT' @'END_STRUCT'.

swap_structfields :- swap_struct_field swap_structfields.
swap_structfields :- .

swap_fieldname :- * &'push'.

swap_struct_field :- 'SELECT_FIELD' swap_fieldname swap_val 'SET_FIELD'
                     @'SELECT_FIELD' &'pop' @'SET_FIELD'
                     .


typetag_name :- * &'push'.

typetag_int :- 'PUSH_INT' * 'SELECT_FIELD' typetag_name &'' 'SET_FIELD'
               @'CHECK_TYPE' &'pop' @'Int' @'SET_FIELD'.

typetag_real :- 'PUSH_REAL' * 'SELECT_FIELD' typetag_name &'' 'SET_FIELD'
                @'CHECK_TYPE' &'pop' @'Real' @'SET_FIELD'.

typetag_sym :- 'PUSH_SYM' * 'SELECT_FIELD' typetag_name &'' 'SET_FIELD'
                @'CHECK_TYPE' &'pop' @'Sym' @'SET_FIELD'.

typetag_bool :- 'PUSH_BOOL' * 'SELECT_FIELD' typetag_name &'' 'SET_FIELD'
                @'CHECK_TYPE' &'pop' @'Bool' @'SET_FIELD'.

typetag_structfields :- typetag_field typetag_structfields.
typetag_structfields :- .

typetag_struct_x :- 'START_STRUCT' @'START_STRUCT' typetag_structfields 'END_STRUCT' @'END_STRUCT'.

typetag_name_x :- * &'' &'push'.

typetag_struct :- 'SET_TYPE' @'SET_TYPE' 
                  typetag_name_x 
                  typetag_struct_x 
                  'SELECT_FIELD' @'SELECT_FIELD'
                  typetag_name_x 
                  @'CHECK_TYPE' &'pop' &'pop'
                  'SET_FIELD' @'SET_FIELD'.

typetag_field :- typetag_int.
typetag_field :- typetag_real.
typetag_field :- typetag_sym.
typetag_field :- typetag_bool.
typetag_field :- typetag_struct.

all :- swap_struct_field all.
all :- typetag_field all.
all :- * &'' all.
all :- .

main :- all.

