
:- include 'system.metal'

ident_x :- any_letter ident_x.
ident_x :- .

ident :- locase_letter ident_x @'FIELD_NAME' &''.

typename :- upcase_letter ident_x.

typename_field :- typename @'FIELD_TYPE' &''.

opttype :- spaces ':' spaces typename_field.
opttype :- spaces @'EMPTY_TYPE'.

structfields_x :- spaces ',' spaces ident opttype structfields_x.
structfields_x :- spaces .

structfields :- spaces ident opttype structfields_x.

typename_def :- typename @'DEF_NAME' &''.

structdef :- spaces 'def' @'START_DEF' spaces '{' structfields '}' spaces typename_def spaces ';' @'END_DEF'.


structvalfields :- spaces ident spaces '=' spaces val spaces ';' structvalfields.
structvalfields :- .

structval :- typename spaces '{' structvalfields '}'.

uintval_x :- digit intval_x.
uintval_x :- .
uintval :- digit intval_x.
uintval :- digit intval_x.

intval :- '-' uintval.
intval :- uintval.

realval :- intval '.' uintval.
realval :- intval 'f'.

symval_x :- {\\} {'} symval_x.
symval_x :- * symval_x.
symval_x :- .
symval :- {'} symval_x {'}.

nilval :- 'nil'.

val :- intval.
val :- realval.
val :- symval.
val :- nilval.

expr :- spaces val spaces ';'.


all :- structdef all.
all :- expr all.
all :- spaces.

main :- all.

