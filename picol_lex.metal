
:- include 'system.metal'

ident_x :- any_letter ident_x.
ident_x :- .

ident :- locase_letter ident_x.

typename :- upcase_letter ident_x.

ident_field :- ident @'FIELD_NAME' &''.
typename_field :- typename @'FIELD_TYPE' &''.

opttype :- spaces ':' spaces typename_field.
opttype :- spaces @'EMPTY_TYPE'.

structfields_x :- spaces ',' spaces ident_field opttype structfields_x.
structfields_x :- spaces .

structfields :- spaces ident_field opttype structfields_x.

typename_def :- typename @'DEF_NAME' &''.

structdef :- spaces 'def' @'START_DEF' spaces '{' structfields '}' spaces typename_def spaces ';' @'END_DEF'.


ident_val :- @'_find_field_idx' ident &''.

structvalfields :- spaces ident_val spaces '=' spaces val spaces ';' @'SET_FIELD' structvalfields.
structvalfields :- .

typename_constructor :- @'_set_type' typename &''.

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
symval_x :- * &'append' symval_x.
symval_x :- {'}.
symval :- @'' {'} symval_x.

nilval :- 'nil'.

val :- @'PUSH_REAL' realval.
val :- @'PUSH_INT' intval &''.
val :- @'PUSH_SYM' symval.
val :- nilval @'PUSH_INT' @'0'.
val :- structval.

expr :- spaces val spaces ';'.


all :- structdef all.
all :- expr all.
all :- spaces.

main :- all.

