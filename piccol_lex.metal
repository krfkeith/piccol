
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

ident_here :- ident &''.


typename :- upcase_letter ident_x.

tuplename_x :- @' ' &'combine' typename_here &'combine' spaces tuplename_x.
tuplename_x :- .

tuplename :- '[' &'append' spaces tuplename_x ']' @' ]' &'combine'.

typename_here :- typename &''.
typename_here :- &'push' tuplename &'pop'.

opttype :- spaces ':' spaces @'FIELD_TYPE' typename_here.
opttype :- spaces @'EMPTY_TYPE'.

structfields :- spaces @'FIELD_NAME' ident_here opttype structfields.
structfields :- spaces .

tuptype :- @' ' &'combine' typename_here &'combine_keep'.

tuplefields :- spaces @'FIELD_NAME' ident_here spaces ':' spaces @'FIELD_TYPE' tuptype tuplefields.
tuplefields :- spaces .

structdef :- '{' structfields '}' 
             spaces @'DEF_NAME' typename_here spaces ';' @'END_DEF'.

tupledef :- '[' &'push' tuplefields ']' @' ]' &'combine' spaces ';'
            spaces @'DEF_NAME' &'pop' @'END_DEF'.

defhead :- spaces 'def' @'START_DEF' spaces.

def :- defhead structdef.
def :- defhead tupledef.



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
symval_x :- {'} &'pop'.
symval_x :- \any &'append' symval_x.
symval :- &'push' {'} symval_x.

nilval :- 'nil'.
nilval :- 'false'.
trueval :- 'true'.


ident_struct_field :- ident &'push'.

structvalfields :- spaces ident_struct_field spaces '=' spaces val_or_call 
                   @'SELECT_FIELD' &'pop' @'SET_FIELD' 
                   structvalfields.

structvalfields :- spaces.

structval :- @'SET_TYPE' typename_here spaces '{' @'START_STRUCT' structvalfields '}' @'END_STRUCT'.


tuplevalfields :- spaces val_or_call @'SET_TUPLEFIELD' tuplevalfields.
tuplevalfields :- spaces.

tupleval :- spaces '[' @'START_TUPLE' tuplevalfields ']' @'END_TUPLE'.


val :- @'SET_TYPE' @'Real' @'PUSH' realval.
val :- @'SET_TYPE' @'Int'  @'PUSH' intval &''.
val :- @'SET_TYPE' @'Sym'  @'PUSH' symval.
val :- nilval  @'SET_TYPE' @'Bool' @'PUSH' @'0'.
val :- trueval @'SET_TYPE' @'Bool' @'PUSH' @'1'.

val_primitive :- val.

val_or_call :- val_primitive.
val_or_call :- structval.
val_or_call :- '(' spaces statements spaces ')'.


funcall :- @'ASMCALL' '$' ident_here.
funcall :- @'CALL' ident_here spaces '->' spaces typename_here.
funcall :- @'CALL' ident_here @'Void'.

statements :- tupleval spaces statements.
statements :- structval spaces statements.
statements :- val_primitive spaces statements.
statements :- funcall spaces statements.
statements :- .


fun :- spaces @'FUN_TYPE' ident_here
       spaces typename_here spaces '->' 
       spaces typename_here 
       spaces ':-' 
       @'START_FUN' spaces statements 
       spaces '.' @'END_FUN'.


all :- def all.
all :- fun all.
all :- spaces.

prelude :- @'START_DEF' @'DEF_NAME' @'Void' @'END_DEF'
           @'START_DEF' @'FIELD_NAME' @'v' @'FIELD_TYPE' @'Int'  @'DEF_NAME' @'Int' @'END_DEF'
           @'START_DEF' @'FIELD_NAME' @'v' @'FIELD_TYPE' @'UInt' @'DEF_NAME' @'UInt' @'END_DEF'
           @'START_DEF' @'FIELD_NAME' @'v' @'FIELD_TYPE' @'Sym'  @'DEF_NAME' @'Sym' @'END_DEF'
           @'START_DEF' @'FIELD_NAME' @'v' @'FIELD_TYPE' @'Bool' @'DEF_NAME' @'Bool' @'END_DEF'
           @'START_DEF' @'FIELD_NAME' @'v' @'FIELD_TYPE' @'Real' @'DEF_NAME' @'Real' @'END_DEF'.


main :- prelude all.

