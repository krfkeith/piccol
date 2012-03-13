
:- comment {

   This code inserts a tag to check that field types match:

   SELECT_FIELD <name> 
    ==>
   SELECT_FIELD <name> CHECK_TYPE <name> 

}.


typetag_name_x :- \any &'' &'push'.

typetag_field :- 'SELECT_FIELD' \any 'CHECK_TYPE' \any &''.

typetag_field :- 'SELECT_FIELD' @'SELECT_FIELD' typetag_name_x  @'CHECK_TYPE' &'pop'.


all :- typetag_field all.
all :- \any &'' all.
all :- .

main :- all.

