
:- comment {

   This code inserts a tag to check that field types match:

   SELECT_FIELD <name> 
    ==>
   SELECT_FIELD <name> CHECK_TYPE <name> 

   It also detects and marks tailcalls:

   CALL <name> <toshape> END_BRANCH END_FUN
    ==>
   TAILCALL <name> <toshape> END_BRANCH END_FUN

   CALL <name> <toshape> [END_LAMBDA|END_BRANCH]* END_FUN
    ==>
   TAILCALL <name> <toshape> [END_BRANCH|END_LAMBDA]* END_FUN

}.


typetag_name_x :- \any &'' &'push'.

typetag_field :- 'SELECT_FIELD' \any 'CHECK_TYPE' \any &''.

typetag_field :- 'SELECT_FIELD' @'SELECT_FIELD' typetag_name_x  @'CHECK_TYPE' &'pop'.

tailcall_name :- \any &''.

tailcall_finishes_x :- 'END_BRANCH' tailcall_finishes_x.
tailcall_finishes_x :- 'END_LAMBDA' tailcall_finishes_x.
tailcall_finishes_x :- .
tailcall_finishes :- 'END_BRANCH' tailcall_finishes_x &''.

tailcall :- 'CALL' @'TAILCALL' tailcall_name tailcall_name tailcall_finishes 'END_FUN' @'END_FUN'.
tailcall :- 'CALL' @'TAILCALL' tailcall_name tailcall_name tailcall_finishes 'END_FUN' @'END_FUN'.


all :- typetag_field all.
all :- tailcall all.
all :- \any &'' all.
all :- .

main :- all.

