
:- include 'system.metal'

ident_x :- any_letter ident_x.
ident_x :- .

ident :- locase_letter ident_x &'FIELD_NAME'.

typename :- upcase_letter ident_x.

typename_field :- typename &'FIELD_TYPE'.

opttype :- spaces ':' spaces typename_field.
opttype :- spaces @'EMPTY_TYPE'.

structfields_x :- spaces ',' spaces ident opttype structfields_x.
structfields_x :- spaces .

structfields :- spaces ident opttype structfields_x.

typename_def :- typename &'DEF_NAME'.

structdef :- spaces 'def' @'START_DEF' spaces '{' structfields '}' spaces typename_def spaces ';' @'END_DEF'.

all :- structdef spaces main.
all :- .

main :- all.

