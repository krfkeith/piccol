
:- include 'system.metal'

ident_x :- any_letter ident_x.
ident_x :- digit ident_x.
ident_x :- '_' ident_x.
ident_x :- .

ident :- locase_letter ident_x.

typename :- upcase_letter ident_x.

ident_field :- ident @'FIELD_NAME' &''.
typename_field :- typename @'FIELD_TYPE' &''.

opttype :- spaces ':' spaces typename_field.
opttype :- spaces @'EMPTY_TYPE'.

structfields :- spaces ident_field opttype structfields.
structfields :- spaces .

typename_def :- typename @'DEF_NAME' &''.

structdef :- spaces 'def' @'START_DEF' spaces '{' structfields '}' 
             spaces typename_def spaces ';' @'END_DEF'.





ident_struct_field :- ident &'push'.

structvalfields :- spaces ident_struct_field spaces '=' spaces val_or_call 
                   @'SELECT_FIELD' &'pop' @'SET_FIELD' 
                   structvalfields.

structvalfields :- spaces.

typename_constructor :- @'SET_TYPE' typename &''.

structval :- typename_constructor spaces '{' @'START_STRUCT' structvalfields '}' @'END_STRUCT'.


uintval_x :- digit uintval_x.
uintval_x :- .
uintval :- digit uintval_x.
uintval :- digit uintval_x.

intval :- '-' uintval.
intval :- uintval.

realval :- intval '.' uintval &''.
realval :- intval &'' 'f'.

symval_x :- {\\} {'} @{'}  &'combine' symval_x.
symval_x :- {\\} {t} @{\t} &'combine' symval_x.
symval_x :- {\\} {n} @{\n} &'combine' symval_x.
symval_x :- {'}.
symval_x :- * &'append' symval_x.
symval :- @'' {'} symval_x.

nilval :- 'nil'.
nilval :- 'false'.
trueval :- 'true'.

val :- @'SET_TYPE' @'Real' @'PUSH' realval.
val :- @'SET_TYPE' @'Int'  @'PUSH' intval &''.
val :- @'SET_TYPE' @'Sym'  @'PUSH' symval.
val :- nilval  @'SET_TYPE' @'Bool' @'PUSH' @'0'.
val :- trueval @'SET_TYPE' @'Bool' @'PUSH' @'1'.
val :- structval.


typename_call :- typename &''.

val_or_call :- val spaces '->' spaces @'CALL' typename_call.
val_or_call :- val.


expr :- spaces structval spaces ';' @'SYSCALL'.

comment_x :- '\n'.
comment_x :- * comment_x.
comment :- spaces '#' comment_x.

all :- structdef all.
all :- expr all.
all :- comment all.
all :- spaces.

main :- all.

