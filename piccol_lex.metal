
space :- ' '.
space :- '\n'.
space :- '\t'.

comment_x :- '\n'.
comment_x :- \any comment_x.
comment_xx :- '*/'.
comment_xx :- \any comment_xx.

comment :- '#' comment_x.
comment :- '/*' comment_xx.

spaces :- space spaces.
spaces :- comment spaces.
spaces :- .

any_letter :- \upcase.
any_letter :- \locase.

upcase_letter :- \upcase.
locase_letter :- \locase.

digit :- \digit.


ident_x :- any_letter ident_x.
ident_x :- digit ident_x.
ident_x :- '_' ident_x.
ident_x :- .

ident :- locase_letter ident_x.

typename :- upcase_letter ident_x.

ident_here :- ident &''.
typename_here :- typename &''.

opttype :- spaces ':' spaces @'FIELD_TYPE' typename_here.
opttype :- spaces @'EMPTY_TYPE'.

structfields :- spaces @'FIELD_NAME' ident_here opttype structfields.
structfields :- spaces .

structdef :- spaces 'def' @'START_DEF' spaces '{' structfields '}' 
             spaces @'DEF_NAME' typename_here spaces ';' @'END_DEF'.





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
symval_x :- \any &'append' symval_x.
symval :- @'' {'} symval_x.

nilval :- 'nil'.
nilval :- 'false'.
trueval :- 'true'.


ident_struct_field :- ident &'push'.

structvalfields :- spaces ident_struct_field spaces '=' spaces val_or_call 
                   @'SELECT_FIELD' &'pop' @'SET_FIELD' 
                   structvalfields.

structvalfields :- spaces.

structval :- @'SET_TYPE' typename_here spaces '{' @'START_STRUCT' structvalfields '}' @'END_STRUCT'.



val :- @'SET_TYPE' @'Real' @'PUSH' realval.
val :- @'SET_TYPE' @'Int'  @'PUSH' intval &''.
val :- @'SET_TYPE' @'Sym'  @'PUSH' symval.
val :- nilval  @'SET_TYPE' @'Bool' @'PUSH' @'0'.
val :- trueval @'SET_TYPE' @'Bool' @'PUSH' @'1'.
val :- structval.


val_or_call :- val spaces '->' spaces @'CALL' @'_cast' typename_here.
val_or_call :- val.
val_or_call :- funcall.


expr :- spaces structval.

funcall :- spaces @'CALL' ident_here spaces '->' spaces typename_here.
funcall :- spaces @'CALL' ident_here @'Void'.

statements :- expr statements.
statements :- funcall statements.
statements :- spaces '.' .


fun :- spaces @'FUN_TYPE' ident_here
       spaces typename_here spaces '->' 
       spaces typename_here 
       spaces ':-' 
       @'START_FUN' statements @'END_FUN'.


all :- structdef all.
all :- fun all.
all :- spaces.

prelude :- @'START_DEF' @'DEF_NAME' @'Void' @'END_DEF'.

main :- prelude all.

