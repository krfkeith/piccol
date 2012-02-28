
:- include 'system.metal'

int_x :- digit int_x.
int_x :- digit .

number_dot :- '.'.

real_x :- int_x number_dot int_x.

int :- int_x &'integer'.

real :- real_x &'real'.

var_x :- any_letter var_x.
var_x :- digit var_x.
var_x :- '_' var_x.
var_x :- .

var :- any_letter var_x.

elt :- ( spaces expr spaces ).
elt :- real.
elt :- int.
elt :- var.

neg :- '-' spaces elt @'neg'.

neg :- elt.

expr_bnot :- '~' spaces neg @'bnot'.
expr_bnot :- neg.

expr_bop :- @'(' expr_bnot spaces '&' spaces expr_bop @'band' @')'.
expr_bop :- @'(' expr_bnot spaces '|' spaces expr_bop @'bor' @')'.
expr_bop :- @'(' expr_bnot spaces '^' spaces expr_bop @'bxor' @')'.
expr_bop :- @'(' expr_bnot @')'.

expr_bsh :- @'(' expr_bop spaces '>>' spaces int @'bshl' @')'.
expr_bsh :- @'(' expr_bop spaces '<<' spaces int @'bshr' @')'.
expr_bsh :- @'(' expr_bop @')'.

expr_m :- @'(' expr_bsh spaces * spaces expr_m @'mul' @')'.
expr_m :- @'(' expr_bsh spaces / spaces expr_m @'div' @')'.
expr_m :- @'(' expr_bsh @')'.

expr_a :- @'(' expr_m spaces + spaces expr_a @'add' @')'.
expr_a :- @'(' expr_m spaces - spaces expr_a @'sub' @')'.
expr_a :- @'(' expr_m @')'.

expr :- spaces expr_a spaces.

var_lval :- any_letter var_x.

statement :- spaces var_lval spaces '=' expr.

statement :- expr.

statements :- statement ';' statements.
statements :- statement ';' spaces.

scope :- spaces 
         '{' 
         statements
         '}' 
         spaces
         .

all :- scope all.
all :- scope.

main :- all.


