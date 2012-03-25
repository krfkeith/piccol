
:- comment {

   This code inserts a tag to check that field types match:

   SELECT_FIELD <name> 
    ==>
   SELECT_FIELD <name> CHECK_TYPE <name> 


   It also detects and marks tailcalls:

   CALL <x> <y> <z> END_BRANCH [END_FUN|END_LAMBDA]
     ==>
   TAILCALL <x> <y> <z> END_BRANCH [END_FUN|END_LAMBDA]

   START_LAMBDA ... END_LAMBDA END_BRANCH [END_FUN|END_LAMBDA]
     ==>
   START_TAIL_LAMBDA ... END_LAMBDA END_BRANCH [END_FUN|END_LAMBDA]

}.


typetag_name_x :- \any &'' &'push'.

typetag_field :- 'SELECT_FIELD' \any 'CHECK_TYPE' \any &''.

typetag_field :- 'SELECT_FIELD' @'SELECT_FIELD' typetag_name_x  @'CHECK_TYPE' &'pop'.


end_branch :- 'END_BRANCH' @'END_BRANCH'.
end_fun :- 'END_FUN' @'END_FUN'.
end_lambda :- 'END_LAMBDA' @'END_LAMBDA'.
end_fun_or_lambda :- end_fun.
end_fun_or_lambda :- end_lambda.

lambda_x :- 'START_LAMBDA' @'START_LAMBDA' lambda_x lambda_x.
lambda_x :- end_lambda.
lambda_x :- \any &'' lambda_x.
lambda :- 'START_LAMBDA' @'START_TAIL_LAMBDA' lambda_x.

tailcall_lambda :- lambda end_branch end_fun_or_lambda.

tailcall_name :- \any &''.

tailcall_call :- 'CALL' @'TAILCALL' tailcall_name tailcall_name 
                 end_branch end_fun_or_lambda.

tailcall :- tailcall_call.
tailcall :- tailcall_lambda.


all :- typetag_field all.
all :- tailcall all.
all :- \any &'' all.
all :- .

main :- all.


