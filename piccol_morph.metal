
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

   
   And inserts a required 'Void' where syntax allows for it to be omitted for clarity.

   OPTIONAL_VOID [CALL|ASMCALL|START_LAMBDA|END_BRANCH]
     ==>
   VOID [CALL|ASMCALL|START_LAMBDA|END_BRANCH]

}.


typetag_name_x :- \any &'' &'push'.

typetag_field :- 'SELECT_FIELD' \any 'CHECK_TYPE' \any &''.

typetag_field :- 'SELECT_FIELD' @'SELECT_FIELD' typetag_name_x  @'CHECK_TYPE' &'pop'.


end_branch :- 'END_BRANCH' @'END_BRANCH'.
end_fun :- 'END_FUN' @'END_FUN'.
end_lambda :- 'END_LAMBDA' @'END_LAMBDA'.

end_fun_or_lambda :- end_branch end_fun.
end_fun_or_lambda :- end_branch end_lambda end_fun_or_lambda.

lambda_x :- 'START_LAMBDA' @'START_LAMBDA' lambda_x lambda_x.
lambda_x :- end_lambda.
lambda_x :- \any &'' lambda_x.
lambda :- 'START_LAMBDA' @'START_TAIL_LAMBDA' lambda_x.

tailcall_lambda :- lambda end_fun_or_lambda.

tailcall_name :- \any &''.

tailcall_call :- 'CALL' @'TAILCALL' tailcall_name tailcall_name 
                 end_fun_or_lambda.

tailcall :- tailcall_call.
tailcall :- tailcall_lambda.

optional_void_call_x :- 'CALL'.
optional_void_call_x :- 'ASMCALL'.
optional_void_call_x :- 'START_LAMBDA'.
optional_void_call_x :- 'END_BRANCH'.
optional_void_call :- optional_void_call_x &''.

optional_void :- 'OPTIONAL_VOID' @'VOID' optional_void_call.
optional_void :- 'OPTIONAL_VOID'.

all :- typetag_field all.
all :- tailcall all.
all :- optional_void all.
all :- \any &'' all.
all :- .

main :- all.


