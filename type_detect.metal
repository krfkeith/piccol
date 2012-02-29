


int_type  :- 'integer' ':'.
real_type :- 'real' ':'.

one_int  :- int_type  atom_or_paren.
one_real :- real_type atom_or_paren.

make_int_op  :- @'integer' @':' @'('.
make_real_op :- @'real' @':' @'('.

binop :- 'add'.
binop :- 'sub'.
binop :- 'mul'.
binop :- 'div'.
binop :- 'band'.
binop :- 'bor'.
binop :- 'bxor'.
binop :- 'bshl'.
binop :- 'bshr'.

uop :- 'neg'.
uop :- 'bnot'.

make_int_binop  :- @'integer' @':' binop &''.
make_real_binop :- @'real'    @':' binop &''.

make_int_uop  :- @'integer' @':' uop &''.
make_real_uop :- @'real'    @':' uop &''.

one_x_int  :- one_int &''.
one_x_real :- one_real &''.

untyped_op :- '(' make_int_op  one_x_int  make_int_uop  ')' @')'.
untyped_op :- '(' make_real_op one_x_real make_real_uop ')' @')'.

untyped_op :- '(' make_int_op  one_x_int  one_x_int  make_int_binop  ')' @')'.
untyped_op :- '(' make_real_op one_x_real one_x_int  make_real_binop ')' @')'.
untyped_op :- '(' make_real_op one_x_int  one_x_real make_real_binop ')' @')'.
untyped_op :- '(' make_real_op one_x_real one_x_real make_real_binop ')' @')'.


close_atom_or_paren :- ')'.
close_atom_or_paren :- op close_atom_or_paren.

atom_or_paren :- '(' close_atom_or_paren.
atom_or_paren :- *.

typed_op :- one_int.
typed_op :- one_real.

op :- typed_op.
op :- untyped_op.

op_x :- typed_op &''.
op_x :- untyped_op.

any :- * &''.

all :- op_x all.
all :- any all.
all :-.

main :- all.
